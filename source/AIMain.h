#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ChatController.h"
#include "ContentProcessor.h"
#include "AudioProcessor.h"

class AIMain {
private:
	static ChatController chatController;
	static ContentProcessor contentProcessor;
	static AudioProcessor audioProcessor;

	static bool checkValid(AIPed* aiPed) {
		if (aiPed == nullptr) {
			return false;
		}
		if (aiPed->isValid()) {
			return true;
		}
		return false;
	}

	static void cleanAIPedPool() {
		// clean up the pool
		for (AIPed* aiPed : AIPedPool) {
			if (!checkValid(aiPed)) {
				AIPedPool.erase(find(AIPedPool.begin(), AIPedPool.end(), aiPed));
				delete aiPed;
				aiPed = nullptr;
				Log::printInfo("Deleted invalid AIPed");
			}
		}
	}

	static void pipeline() {
		cleanAIPedPool();
		contentProcessor.processContent();
		audioProcessor.addAudio();
		audioProcessor.processAudio();
	}

public:
	static void install() { // install when init
		Events::initGameEvent += [] {
			AIPed* playerAIPed = new AIPed(FindPlayerPed(), "CARL");
			AIPedPool.push_back(playerAIPed);
			};
		ChatController::install();

		Events::gameProcessEvent.Add(pipeline);
	}

	static void uninstall() { // uninstrall when shutdown
		AIPedPool.clear();
		std::lock_guard<std::mutex> lock1(contentMutex);
		std::queue<AIBeh*>().swap(contentBuf);
		std::lock_guard<std::mutex> lock2(audioMutex);
		std::queue<AIBeh*>().swap(audioBuf);
		std::lock_guard<std::mutex> lock3(historyMutex);
		history.clear();
	}


	
};