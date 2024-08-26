#pragma once
#include <thread>

#include "CPed.h"
#include "extensions/ScriptCommands.h"

#include "ChatBot.h"
#include "Speak.h"
#include "Subtitle.h"

class AIPed {
private:
	CPed* ped;
	ChatBot chatBot;
	std::string name;
	std::string system_prompt;

	void joinGroup() {
		int gr;
		Command<Commands::GET_PLAYER_GROUP>(0, &gr);
//		Log::printInfo("Get gr");
		if (!Command<Commands::IS_GROUP_MEMBER>(gr, ped)) {
			Log::printInfo("This Ped is not in player's group, add it");
			Command<Commands::SET_GROUP_LEADER>(gr, FindPlayerPed());
			Log::printInfo("reset leader");
			Command<Commands::SET_GROUP_MEMBER>(gr, ped);
			Log::printInfo("set as follower");
		}
	}

public:

	AIPed(CPed* ped, std::string name) {
		this->ped = ped;
		this->name = name;
		this->system_prompt = Config::getSysPrompt(name);
		this->chatBot = ChatBot();
		if (ped != FindPlayerPed()) {
			joinGroup();
		}
	}

	bool operator==(const AIPed& other) const {
		return this->ped == other.ped;
	}

	bool isValid() {
		if (ped == nullptr || !IsPedPointerValid(ped)) {
			return false;
		}
		for (auto gamePed : CPools::ms_pPedPool)
		{
			if (ped == gamePed && ped->IsAlive()) {
				return true;
			}
		}
		return false;
	}

	std::string answer(std::string msg) {
		if (!chatBot.isInitialized()) {
			chatBot.ask(system_prompt, true);
		}
		return chatBot.ask(msg);
	}

	std::string getName() {
		return name;
	}

	CPed* getPed() {
		return ped;
	}
};