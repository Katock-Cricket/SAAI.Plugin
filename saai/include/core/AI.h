#pragma once

#include <thread>

#include "CPed.h"

#include "ChatBot.h"
#include "Speak.h"
#include "Subtitle.h"


// added in CPed class as a "void*" member, must be used by a ped pointer
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

    bool operator==(const AI &other) const {
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

    static void addAIForPed(CPed *ped) { // add ai for normal peds and cj, not for special npc
        if (ped == nullptr || !IsPedPointerValid(ped)) {
            return;
        }
//        Log::printInfo("ai = " + std::to_string((int)ped->ai));
        if (ped->ai != nullptr) {
//            Log::printInfo("ped->ai is not null");
            return;
        }
//        Log::printInfo("create ai for ped");
        AI *ai = new AI();
        if (ped == FindPlayerPed()) {
            ai->setName("Carl");
//            Log::printInfo("add ai for cj");
        }
        ped->ai = ai;
//        Log::printInfo("addr of ai when create: " + std::to_string(reinterpret_cast<uintptr_t>(ped->ai)));
//        Log::printInfo("get name when create: " + static_cast<AI *>(ped->ai)->getName());
    }

    static void install() {
        Events::pedRenderEvent.before.Add(addAIForPed);
    }
};