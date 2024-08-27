#pragma once
#include "CWorld.h"
#include "CModelInfo.h"
#include "CVehicle.h"
#include "CBike.h"
#include "CStreaming.h"
#include "CTaskComplexWanderStandard.h"
#include "CHud.h"

#include "CheatActivate.h"
#include "KeyActivate.h"
#include "Subtitle.h"
#include "Speak.h"
#include "Config.h"
#include "ThreadPool.h"

class UnitTest : public CheatActivate, public KeyActivate {
public:
	UnitTest() {}

	static void install() {
		Events::gameProcessEvent.Add(testNRG);
		Events::gameProcessEvent.Add(testSpeak);
        Events::gameProcessEvent.Add(testSubtitle);
	}

private:
    static void printSubtitleInOrder() {
        Subtitle::printSubtitle("Test subtitle");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        Subtitle::printSubtitle("测试硬编码的中文");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        Subtitle::printSubtitle("UTF8: " + Config::getMeetPrompt());
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        Subtitle::forceStopRender();
    }

    static void testSubtitle() {
        if (cheat_pressed("sub")) {
            ThreadPool::start(&printSubtitleInOrder);
        }
    }

	static void addSpeak() {
		AudioPath audioPath = { true, "SCRIPT", 42, 2 };
		Speak::autoAddSpeak(FindPlayerPed(), audioPath, "Test very long audio to speak");
	}

	static void testSpeak() {
		if (cheat_pressed("speak")) {
			ThreadPool::start(&addSpeak);
		}
	}

	static void testNRG() {
		if (FindPlayerPed() && key_pressed(VK_F10)) {
			CVector spawnPos = FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 3.0f, -0.5f));
			CVector spawnOri = CVector();
			FindPlayerPed()->GetOrientation(spawnOri.x, spawnOri.y, spawnOri.z);

			int modelIndex = 522;
			CStreaming::RequestModel(modelIndex, 1);
			CStreaming::LoadAllRequestedModels(false);

			CVehicle* vehicle = new CBike(modelIndex, 1);
			vehicle->SetPosn(spawnPos);
			vehicle->SetOrientation(spawnOri.x, spawnOri.y, spawnOri.z);

			if (vehicle) {
				CWorld::Add(vehicle);
				CHud::SetHelpMessage("SpawnVEH", true, false, false);
			}
		}
	}
};