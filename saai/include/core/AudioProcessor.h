#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ThreadPool.h"

class AudioProcessor {
private:

    static void deleteAIBehFromAudioBuf(AIBeh* aiBeh){
        for(auto it = audioBuf.begin(); it != audioBuf.end(); it++){
            if(**it == *aiBeh){
                audioBuf.erase(it);
                delete aiBeh;
                aiBeh = nullptr;
                return;
            }
        }
    }

	static void generateAudio(AIBeh* aiBeh) {
		Log::printInfo("in generate audio");
		CPed* ped = aiBeh->getPed();
        if (ped == nullptr || !IsPedPointerValid(ped)) {
            std::lock_guard<std::mutex> lock(audioMutex);
            deleteAIBehFromAudioBuf(aiBeh);
            return;
        }
		std::string name = ((AI*)ped->ai)->getName();
		std::string content = aiBeh->getContent();
		AudioPath audioPath = SVCClient::request_audio(content, name); // time costing
		if (!audioPath.valid) {
			Log::printError("invalid audioPath");
			return;
		}
		while (!Speak::canAddSpeak()) { //may cost time
			Log::printInfo("waiting for speaker");
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		std::unique_lock<std::mutex> lock(audioMutex);
        deleteAIBehFromAudioBuf(aiBeh);
		lock.unlock();
		Speak::addSpeak(ped, audioPath, content, aiBeh->getIsFirstOfFreeChat()); // time costing
	}

    static AIBeh* findNextBeh() {
        for(auto &aiBeh : audioBuf) {
            if(!aiBeh->isWorking()){
                return aiBeh;
            }
        }
        return nullptr;
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
        if (aiBeh->getContent().empty()) {
            return;
        }
		contentBuf.pop();
        aiBeh->finish();
		if (aiBeh->getContent() == "Error") {
			Log::printError("Do not gen audio for any 'Error' ");
			return;
		}
		audioBuf.push_back(aiBeh);
		Log::printInfo("Add a beh for audio generation");
		Log::printInfo("================================");
	}

	static void processAudio() {
		std::lock_guard<std::mutex> lock(audioMutex);
		if (audioBuf.empty()) {
			//Log::printInfo("AudioBuf is empty, no audio to generate");
			return;
		}
		AIBeh* aiBeh = findNextBeh();
		if (aiBeh == nullptr) {
			return;
		}
		aiBeh->start();
		ThreadPool::start(&generateAudio, aiBeh);
	}
};