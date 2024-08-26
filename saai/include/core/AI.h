#pragma once
#include <thread>

#include "CPed.h"
#include "extensions/ScriptCommands.h"

#include "ChatBot.h"
#include "Speak.h"
#include "Subtitle.h"


// added in CPed class as a "void*" member, must be used with a ped pointer
class AI {
private:
	ChatBot chatBot;
	std::string name;
	std::string system_prompt;

public:

	AI() {
        this->name = "";
        this->system_prompt = "";
		this->chatBot = ChatBot();
	}

    void joinGroup(CPed* ped) {
        if(ped == nullptr || !IsPedPointerValid(ped) || ped == FindPlayerPed()) {
            return;
        }
        int gr;
        Command<Commands::GET_PLAYER_GROUP>(0, &gr);
        if (!Command<Commands::IS_GROUP_MEMBER>(gr, ped)) {
            Log::printInfo("This Ped is not in player's group, add it");
            Command<Commands::SET_GROUP_LEADER>(gr, FindPlayerPed());
            Log::printInfo("reset leader");
            Command<Commands::SET_GROUP_MEMBER>(gr, ped);
            Log::printInfo("set as follower");
        }
    }

	bool operator==(const AI& other) const {
		return this->name == other.name;
	}

	std::string answer(std::string msg) {
		if (!chatBot.isInitialized()) {
			chatBot.ask(system_prompt, true);
		}
		return chatBot.ask(msg);
	}

    void setName(std::string name) {
        this->name = name;
        this->system_prompt = Config::getSysPrompt(name);
    }

	std::string getName() {
		return name;
	}

    static void addAIForPed(CPed* ped) { // add ai for normal peds and cj, not for special npc
        if(ped == nullptr || !IsPedPointerValid(ped)){
            return;
        }
//        Log::printInfo("ai = " + std::to_string((int)ped->ai));
        if(ped->ai != 0) {
//            Log::printInfo("ped->ai is not null");
            return;
        }
        AI* ai = new AI();
        if(ped == FindPlayerPed()) {
            ai->setName("Carl");
            Log::printInfo("add ai for cj");
        }
        ped->ai = ai;
    }

    static void install() {
        Events::pedRenderEvent.before.Add(addAIForPed);
    }
};