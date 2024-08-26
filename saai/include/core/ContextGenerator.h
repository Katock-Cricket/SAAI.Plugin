#pragma once
#include <random>
#include <ctime>

#include "CheatActivate.h"
#include "AIBeh.h"
#include "Config.h"
#include "SharedMem.h"
#include "ThreadPool.h"

class ContextGenerator : public CheatActivate {
private:
    static std::vector<CPed*> getPedCanFreeChat () {
        std::vector<CPed*> ret;
        for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++) {
            CPed *ped = CPools::ms_pPedPool->GetAt(i);
            if(ped == nullptr || !IsPedPointerValid(ped)){
                continue;
            }
            if(((AI*)ped->ai) != 0 && ((AI*)ped->ai)->getName()!= ""){ // have ai && is special npc
                ret.push_back(ped);
            }
        }
        return ret;
    }

	static CPed* choosePedToSpeak() {
        std::vector<CPed*> pedCanFreeChat = getPedCanFreeChat();
		if (pedCanFreeChat.size() < 2) {
			return nullptr; // there are less than 2 peds, no conversation
		}

		CPed* lastPed = nullptr;
		std::lock_guard<std::mutex> lock(historyMutex);
		if (history.empty()) {
            lastPed = pedCanFreeChat[0]; //carl is the default first AIPed
		}
		else {
            lastPed = history.back().ped;
		}
		// choose a ped that is different from lastAIPed
		CPed* ped;
		std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
		std::uniform_int_distribution<std::size_t> dist;
		dist = std::uniform_int_distribution<std::size_t>(0, pedCanFreeChat.size() - 1);
		do {
			std::size_t random_index = dist(rng);
            ped = pedCanFreeChat[random_index];
		} while (ped == lastPed);
		return ped;
	}

	static std::string generateContext(CPed* pedToSpeak) {
		std::lock_guard<std::mutex> lock(historyMutex);
		if (noValidRecord()) { // generate context that starts a chat
			std::string nameBuffer = "";
			for (CPed* ped : getPedCanFreeChat()) {
				if (ped == pedToSpeak) {
					continue;
				}
				nameBuffer += ((AI*)ped->ai)->getName() + ", ";
			}
			return Config::getMeetPrompt() + nameBuffer;
		}
		else { // use last Record as the context
			Record* lastRec = lastRecord();
			if (lastRec == nullptr) {
				Log::printError("no last rec");
				return "";
			}
			return lastRec->toString();
		}
	}

	static bool samePrompt(std::string pmt1, std::string pmt2) {
		if (pmt1 == pmt2) {
			return true;
		}
		if (pmt1.size() < 10 || pmt2.size() < 10) {
			return false;
		}
		return pmt1.compare(0, 10, pmt2, 0, 10) == 0;
	}

	static void activateChat() {
		if (cheat_pressed(Config::getCheatFreeChat())) {
			std::lock_guard<std::mutex> lock(chatMutex);
			isChating = true;
			CHud::SetHelpMessage("Free chat launching", true, false, false);
		}
	}

	static void autoDeactivateChat() {
		if (history.size() >= Config::getChatRound()) {
			std::lock_guard<std::mutex> lock(chatMutex);
			isChating = false;
			history.clear();
			//CHud::SetHelpMessage("Free chat is about to over", true, false, false);
		}
	}

	static void pipeline() {
		//Log::printInfo("---------------ContextGenerator Pipeline-----------------");
		autoDeactivateChat();
		//Log::printInfo("autoDeactivateChat");
		activateChat();
		//Log::printInfo("activateChat");
		addContent();
		//Log::printInfo("addContent");
	}

public:
	static void install() {
		Events::gameProcessEvent.Add(pipeline);
	}

	static void addContent() {
		std::lock_guard<std::mutex> lock0(chatMutex);
		if (!isChating) {
			return;
		}

		// add content
		std::lock_guard<std::mutex> lock1(contentMutex);
		if (contentBuf.size() > Config::getContentBufSize()) {
			//Log::printInfo("contentBuf is full, don't add");
			return;
		}
		CPed* pedToSpeak = choosePedToSpeak();
		if (pedToSpeak == nullptr || !IsPedPointerValid(pedToSpeak)) {
			//Log::printInfo("No ped to talk now, CJ is alone");
			return;
		}
		std::string context = generateContext(pedToSpeak);
		if (!contentBuf.empty() && samePrompt(contentBuf.back()->getContext(), context)) {
			//Log::printInfo("duplicate context, can't add beh now");
			return;
		}
		std::lock_guard<std::mutex> lock2(historyMutex);
		history.push_back(Record(pedToSpeak, ((AI*)pedToSpeak->ai)->getName(), ""));
		AIBeh* aiBeh = new AIBeh(pedToSpeak);
		aiBeh->setContext(context);
		contentBuf.push(aiBeh);
		Log::printInfo("Add a beh for content generation");
		Log::printInfo("================================");
	}
};