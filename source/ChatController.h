#pragma once
#include <random>
#include <ctime>

#include "CheatActivate.h"
#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"

class ChatController : public CheatActivate{
private:
	static bool isChating;

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

	static std::string generateContext() {
		std::lock_guard<std::mutex> lock(historyMutex);
		if (history.empty()) {
			return Config::getMeetPrompt();
		}
		else {
			return history.back().toString();
		}
	}

	static void activateChat() {
		if (cheat_pressed("chat")) {
			isChating = true;
			CHud::SetHelpMessage("Free chat is luanching...", true, false, false);
		}
	}

	static void autoDeactivateChat() {
		if (history.size() > 6) {
			isChating = false;
			history.clear();
			CHud::SetHelpMessage("Free chat is over", true, false, false);
		}
	}

	static void pipeline() {
		autoDeactivateChat();
		activateChat();
		if (isChating) {
			addContent();
		}
	}

public:
	static void install() {
		Events::gameProcessEvent.Add(pipeline);
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
};