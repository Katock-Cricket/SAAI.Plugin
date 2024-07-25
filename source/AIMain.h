#pragma once
#include <random>
#include <ctime>

#include "AIBeh.h"
#include "Config.h"


class AIMain {
private:
	class Record {
	public:
		AIPed* aiPed;
		std::string name;
		std::string content;
		Record(AIPed* aiPed, std::string name, std::string content) {
			this->aiPed = aiPed;
			this->name = name;
			this->content = content;
		}
		std::string toString() {
			return name + ": " + content;
		}
	};
	static std::vector<AIPed*> AIPedPool;
	static std::mutex contentMutex;
	static std::mutex audioMutex;
	static std::mutex historyMutex;
	static std::queue<AIBeh*> contentBuf;
	static std::queue<AIBeh*> audioBuf;
	static std::vector<Record> history;

	static bool checkValid(AIPed* aiPed) {
		if (aiPed->isValid()) {
			return true;
		}
		AIPedPool.erase(find(AIPedPool.begin(), AIPedPool.end(), aiPed));
		delete aiPed;
		aiPed = nullptr;
		return false;
	}

	static AIPed* chooseAIPedToSpeak() {
		if (AIPedPool.size() < 2) {
			return nullptr; // there are less than 2 peds, no conversation
		}

		AIPed* lastAIPed = nullptr;
		std::lock_guard<std::mutex> lock(historyMutex);
		if (history.empty()) {
			lastAIPed = AIPedPool[0]; //carl is the default first AIPed
		}
		else {
			lastAIPed = history.back().aiPed;
		}
		// choose a ped that is different from lastAIPed
		AIPed* aiPed;
		std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
		std::uniform_int_distribution<std::size_t> dist;
		dist = std::uniform_int_distribution<std::size_t>(0, AIPedPool.size() - 1);
		do {
			std::size_t random_index = dist(rng);
			aiPed = AIPedPool[random_index];
		} while (aiPed == lastAIPed);
		return aiPed;
	}

	static void printHistory() {
		Log::printInfo("History:---------------------------");
		for (Record rec : history) {
			Log::printInfo(rec.toString());
		}
		Log::printInfo("-----------------------------------\n");
	}

	static std::string generateContext() {
		std::lock_guard<std::mutex> lock(historyMutex);
		if (history.empty()) {
			return "System: You meet the player-controlled protagonist, CJ. Now you say a short, character-appropriate greeting";
		}
		else {
			return history.back().toString();
		}
	}

	static void generateContent(AIBeh* aiBeh) {
		AIPed* aiPed = aiBeh->getAIPed();
		if (aiPed == nullptr || !aiPed->isValid()) {
			Log::printError("invalid aiPed");
			std::lock_guard<std::mutex> lock(contentMutex);
			contentBuf.pop();
			delete aiBeh;
			aiBeh = nullptr;
			return;
		}
		std::string context = aiBeh->getContext();
		std::string content = aiPed->answer(context); // time costing
		aiBeh->setContent(content);
		if (content == "Error") {
			Log::printError("'Error' don't be recorded in history");
			CHud::SetHelpMessage("ChatGPT/HTTP/JSON Error", true, false, false);
			return;
		}
		std::lock_guard<std::mutex> lock(historyMutex);
		history.push_back(Record(aiPed, aiPed->getName(), content));
		printHistory();
	}

	static void generateAudio(AIBeh* aiBeh) {
		AIPed* aiPed = aiBeh->getAIPed();
		if (aiPed == nullptr || !aiPed->isValid()) {
			std::lock_guard<std::mutex> lock(audioMutex);
			audioBuf.pop();
			delete aiBeh;
			aiBeh = nullptr;
			return;
		}
		CPed* ped = aiPed->getPed();
		std::string name = aiPed->getName();
		std::string content = aiBeh->getContent();

		SVCClient client;
		AudioPath audioPath = client.request_audio(content, name); // time costing
		if (!audioPath.valid) {
			return;
		}

		std::unique_lock<std::mutex> lock(audioMutex);
		audioBuf.pop();
		delete aiBeh;
		aiBeh = nullptr;
		lock.unlock();

		Speak::addSpeak(ped, audioPath, content); // time costing
		Log::printInfo("audio generation completed: " + content);
	}


	static void addContent() {
		// add content
		std::lock_guard<std::mutex> lock(contentMutex);
		if (contentBuf.size() > Config::getContentBufSize()) {
			//Log::printInfo("contentBuf is full, don't add");
			return;
		}
		AIPed* aiPedToSpeak = chooseAIPedToSpeak();
		if (aiPedToSpeak == nullptr) {
			//Log::printInfo("No ped to talk now, CJ is alone");
			return;
		}
		std::string context = generateContext();
		if (!contentBuf.empty() && contentBuf.back()->getContext() == context) {
			//Log::printInfo("duplicate context, can't add beh now");
			return;
		}
		AIBeh* aiBeh = new AIBeh(aiPedToSpeak);
		aiBeh->setContext(context);
		contentBuf.push(aiBeh);
		Log::printInfo("Add a beh for content generation");
		Log::printInfo("================================");
	}

	static void processContent() {
		//process a content
		std::lock_guard<std::mutex> lock(contentMutex);
		if (contentBuf.empty()) {
			//Log::printInfo("contentBuf is empty, no content to genarate");
			return;
		}
		AIBeh* aiBeh = contentBuf.front();
		if (aiBeh->isWorking()) {
			//Log::printInfo("The beh is generating content, only 1 beh can work");
			return;
		}
		aiBeh->start();
		std::thread t(&AIMain::generateContent, aiBeh);
		t.detach();
	}

	static void addAudio() {
		std::lock_guard<std::mutex> lock1(audioMutex);
		if (audioBuf.size() > Config::getAudioBufSize()) {
			//Log::printInfo("AudioBuf is full, don't add");
			return;
		}
		std::lock_guard<std::mutex> lock2(contentMutex);
		if (contentBuf.empty()) {
			//Log::printInfo("ContentBuf is empty, nothing to add");
			return;
		}
		AIBeh* aiBeh = contentBuf.front();
		if (aiBeh->getContent() == "") {
			//Log::printInfo("A beh is waiting for content, don't add for now");
			return;
		}
		aiBeh->finish();
		contentBuf.pop();
		if (aiBeh->getContent() == "Error") {
			Log::printError("Do not gen audio for any 'Error' ");
			return;
		}
		audioBuf.push(aiBeh);
		Log::printInfo("Add a beh for audio generation");
		Log::printInfo("================================");
	}	

	static void processAudio() {
		std::lock_guard<std::mutex> lock(audioMutex);
		if (audioBuf.empty()) {
			//Log::printInfo("AudioBuf is empty, no audio to generate");
			return;
		}
		AIBeh* aiBeh = audioBuf.front();
		if (aiBeh->isWorking()) {
			//Log::printInfo("The beh is generating audio, only 1 beh can work");
			return;
		}
		aiBeh->start();
		std::thread t(&AIMain::generateAudio, aiBeh);
		t.detach();
	}

	static void contentBufAdder() {

	}

public:
	static void install() {
		Events::initGameEvent += [] {
			AIPed* playerAIPed = new AIPed(FindPlayerPed(), "carl");
			AIPedPool.push_back(playerAIPed);
			};
		Events::gameProcessEvent += [] {
			// clean up the pool
			for (AIPed* aiPed : AIPedPool) {
				if (!checkValid(aiPed)) {
					Log::printInfo("Deleted invalid AI");
				}
			}
			};

		Events::gameProcessEvent.Add(addContent);
		Events::gameProcessEvent.Add(processContent);
		Events::gameProcessEvent.Add(addAudio);
		Events::gameProcessEvent.Add(processAudio);
	}

	static void addAIPed(CPed* ped, std::string name) {
		AIPed* aiPed = new AIPed(ped, name);
		AIPedPool.push_back(aiPed);
	}
	
};