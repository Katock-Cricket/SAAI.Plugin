#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ContextGenerator.h"
#include "ContentGenerator.h"
#include "AudioProcessor.h"

class AIMain {
private:
	static void pipeline() {
		//Log::printInfo("---------------AIMain Pipeline-----------------");
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
        AI::install();
        ContextGenerator::install();
        Events::gameProcessEvent.Add(pipeline);
	}

	static void uninstall() { // uninstall when shutdown
		std::lock_guard<std::mutex> lock1(contentMutex);
		std::queue<AIBeh*>().swap(contentBuf);
		std::lock_guard<std::mutex> lock2(audioMutex);
		audioBuf.clear();
		std::lock_guard<std::mutex> lock3(historyMutex);
		history.clear();
	}
};