#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ContextGenerator.h"
#include "ContentGenerator.h"
#include "AudioProcessor.h"

class AIMain {
private:
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
		//Log::printInfo("---------------AIMain Pipeline-----------------");
		cleanAIPedPool();
		//Log::printInfo("cleanAIPedPool");
        //Log::printInfo("processContent");
		ContentGenerator::processContent();
		//Log::printInfo("processContent");
		AudioProcessor::addAudio();
		//Log::printInfo("addAudio");
		AudioProcessor::processAudio();
		//Log::printInfo("processAudio");
		//Log::printInfo("Num workers: " + std::to_string(ThreadPool::getWorkingNum()));
	}

public:
	static void install() { // install when init
		Events::initGameEvent += [] {
			AIPed* playerAIPed = new AIPed(FindPlayerPed(), "Carl");
			AIPedPool.push_back(playerAIPed);
			};
        ContextGenerator::install();
        Events::gameProcessEvent.Add(pipeline);
	}

	static void uninstall() { // uninstrall when shutdown
		AIPedPool.clear();
		std::lock_guard<std::mutex> lock1(contentMutex);
		std::queue<AIBeh*>().swap(contentBuf);
		std::lock_guard<std::mutex> lock2(audioMutex);
		audioBuf.clear();
		std::lock_guard<std::mutex> lock3(historyMutex);
		history.clear();
	}
};