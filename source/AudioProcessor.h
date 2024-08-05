#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ThreadPool.h"

class AudioProcessor {
private:
	static void generateAudio(AIBeh* aiBeh) {
		Log::printInfo("in generate audio");
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
		AudioPath audioPath = SVCClient::request_audio(content, name); // time costing
		if (!audioPath.valid) {
			Log::printError("invalid audioPath");
			return;
		}
		while (!Speak::canAddSpeak()) { //may cost time
			Log::printInfo("waiting for speaker");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		std::unique_lock<std::mutex> lock(audioMutex);
		audioBuf.pop();
		delete aiBeh;
		aiBeh = nullptr;
		lock.unlock();
		Speak::addSpeak(ped, audioPath, content); // time costing
	}

public:
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
		if (aiBeh == nullptr) {
			Log::printError("Beh in contentbuf is null, when adding audio");
			contentBuf.pop();
			return;
		}
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
		if (aiBeh == nullptr) {
			Log::printError("beh in audioBuf is null, when precessing audio");
			audioBuf.pop();
			return;
		}
		if (aiBeh->isWorking()) {
			//Log::printInfo("The beh is generating audio, only 1 beh can work");
			return;
		}
		aiBeh->start();
		ThreadPool::start(&generateAudio, aiBeh);
	}
};