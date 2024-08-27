#pragma once

#include <stdexcept>
#include<thread>

#include "plugin.h"
#include "CWorld.h"
#include "CPopulation.h"
#include "CCivilianPed.h"
#include "CStreaming.h"
#include "CTaskComplexWanderStandard.h"
#include "CHud.h"
#include "CFont.h"
#include "CPools.h"

#include "KeyActivate.h"
#include "CheatActivate.h"
#include "AIMain.h"
#include "utils.h"


class PedSpawner : public KeyActivate, public CheatActivate {
private:
    const static int pedModelIds[];
    const static std::map<std::string, int> specialPedSlot;

    static CPed *createPed(int modelID) {
        CStreaming::RequestModel(modelID, 0);
        CStreaming::LoadAllRequestedModels(false);
        CPed *ped = new CCivilianPed(CPopulation::IsFemale(modelID) ? PED_TYPE_CIVFEMALE : PED_TYPE_CIVMALE, modelID);
        return ped;
    }

    static CPed *createSpecialActor(int slot, std::string name) {
        //Command<Commands::LOAD_SPECIAL_CHARACTER>(slot, name.c_str());
        //CStreaming::RequestSpecialChar(slot, name.c_str(), PRIORITY_REQUEST);
        CStreaming::RequestSpecialModel(slot, name.c_str(), PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
        CPed *ped = new CCivilianPed(PED_TYPE_GANG2, slot);
        CStreaming::SetSpecialCharIsDeletable(slot);
        return ped;
    }

    static bool alreadySpawned(std::string pedName) {
        for (auto ped : CPools::ms_pPedPool) {
            if (ped->ai == 0) {
//                Log::printInfo("only check ped with ai, which is set when create ped");
                continue;
            }
            try {
                AI* ai = static_cast<AI*>(ped->ai);
//                Log::printInfo("addr of ai when get again: " + std::to_string(reinterpret_cast<uintptr_t>(ai)));
                if (ai->getName() == pedName) {
                    return true;
                }
            } catch (std::bad_cast& e) {
                Log::printError("bad_cast when get ai: " + std::string(e.what()));
                return true;
            }
        }
        return false;
    }

    static void cheatSpawn() {
        for (auto &t: Config::getCheatNPC()) {
            std::string pedName = t.first;
            std::string modelName(pedName.size(), ' ');
            transform(pedName.begin(), pedName.end(), modelName.begin(), ::toupper);
            std::string pedCheat = t.second;

            if (FindPlayerPed() && CheatActivate::cheat_pressed(pedCheat)) {
                auto it = specialPedSlot.find(modelName);
                if (it == specialPedSlot.end()) {
                    Log::printError("slot not found for special ped error: " + pedName);
                    return;
                }
                if (alreadySpawned(pedName)) {
                    CHud::SetHelpMessage("don't spawn same ped twice", true, false, false);
                    return;
                }
                CPed *ped = createSpecialActor(it->second, modelName);
                ped->SetPosn(FindPlayerPed()->TransformFromObjectSpace(CVector(-2.0f, 0.0f, 0.0f)));
                ped->SetOrientation(0.0f, 0.0f, 0.0f);
                CWorld::Add(ped);
                ped->PositionAnyPedOutOfCollision();
                ped->m_fHealth = 10000;
                ped->m_fArmour = 10000;
                ped->m_nPedFlags.bNoCriticalHits = 1;
                CStreaming::RequestModel(MODEL_AK47, 2);
                CStreaming::LoadAllRequestedModels(false);
                ped->GiveWeapon(WEAPON_AK47, 10000, true);
                //ped->SetCurrentWeapon(WEAPON_AK47);
                CStreaming::SetModelIsDeletable(MODEL_AK47);

                AI *ai = new AI();
                ai->setName(pedName);
                joinGroup(ped);
                ped->ai = ai;

                CHud::SetHelpMessage(std::string("Spawned " + pedName).c_str(), true, false, false);
            }
        }
    }

public:
    PedSpawner() {}

    static void install() {
        Events::gameProcessEvent.Add(cheatSpawn);
    }
};
