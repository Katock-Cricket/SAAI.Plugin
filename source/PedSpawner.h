#pragma once
#include <stdexcept>
#include<thread>

#include "plugin.h"
#include "CWorld.h"
#include "CPopulation.h"
#include "CCivilianPed.h"
#include "CPedGroupMembership.h"
#include "CStreaming.h"
#include "CTaskComplexWanderStandard.h"
#include "CHud.h"
#include "CFont.h"
#include "CPools.h"
#include "CPedGroups.h"

#include "KeyActivate.h"
#include "CheatActivate.h"
#include "Subtitle.h"
#include "Speak.h"
#include "AIMain.h"

class PedSpawner : public KeyActivate, public CheatActivate{
private:
	const static int pedModelIds[];

	static CPed* createPed(int modelID) {
		CStreaming::RequestModel(modelID, 0);
		CStreaming::LoadAllRequestedModels(false);
		CPed* ped = new CCivilianPed(CPopulation::IsFemale(modelID) ? PED_TYPE_CIVFEMALE : PED_TYPE_CIVMALE, modelID);
		return ped;
	}
	static CPed* createSpecialActor(int slot, const char* name) {
		Command<Commands::LOAD_SPECIAL_CHARACTER>(slot, name);
		Log::printInfo("load special character in slot");
		while (true) {
			try {
				CStreaming::RequestSpecialModel(slot, name, 0);
				break;
			}
			catch (const std::runtime_error e) {
				Log::printError("load special model in slot failed");
				continue;
			}
		}
		CStreaming::LoadAllRequestedModels(false);
		CPed* ped = new CCivilianPed(PED_TYPE_GANG2, slot);
		return ped;
	}
	static void cheatSpawn() {
		if (FindPlayerPed() && CheatActivate::cheat_pressed("RYDER")) { 
			CPed* ped = createSpecialActor(290, "RYDER");
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
			addAIPed(ped, "ryder");
			CHud::SetHelpMessage("Spawned Ryder", true, false, false);
		}
	}
public: 
	PedSpawner() {}
	static void install() {         
		Events::gameProcessEvent.Add(cheatSpawn);
	}
};
