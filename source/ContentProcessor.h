#pragma once

#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"


class ContentProcessor {
private:
	static void printHistory() {
		Log::printInfo("History:---------------------------");
		for (Record rec : history) {
			Log::printInfo(rec.toString());
		}
		Log::printInfo("-----------------------------------\n");
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
		Log::printInfo("Start generating content for the beh");
		std::string content = aiPed->answer(context); // time costing
		aiBeh->setContent(content);
		if (content == "Error") {
			Log::printError("'Error' don't be recorded in history");
			CHud::SetHelpMessage("ChatGPT/HTTP/JSON Error", true, false, false);
			return;
		}
		std::lock_guard<std::mutex> lock0(chatMutex);
		if (isChating) {
			std::lock_guard<std::mutex> lock(historyMutex);
			Record* nextEmptyRec = nextEmptyRecord();
			if (nextEmptyRec == nullptr) {
				Log::printError("all recs are full");
				return;
			}
			nextEmptyRec->content = content;
			printHistory();
		}
	}

public:
	static void processContent() {
		//process a content
		std::lock_guard<std::mutex> lock(contentMutex);
		if (contentBuf.empty()) {
			//Log::printInfo("contentBuf is empty, no content to genarate");
			return;
		}
		AIBeh* aiBeh = contentBuf.front();
		if (aiBeh == nullptr) {
			Log::printError("aiBeh in ContentBuf is null, when starting generating content");
			contentBuf.pop();
			return;
		}
		if (aiBeh->isWorking()) {
			//Log::printInfo("The beh is generating content, only 1 beh can work");
			return;
		}
		aiBeh->start();
		Log::printInfo("Start processing a beh, context:");
		Log::printInfo(aiBeh->getContext());
		ThreadPool::start(&generateContent, aiBeh);
	}
};