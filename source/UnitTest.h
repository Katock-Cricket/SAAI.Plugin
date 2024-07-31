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


class UnitTest : public CheatActivate, public KeyActivate {
public:
	UnitTest() {}

	void install() {
		Events::gameProcessEvent.Add(testCHS);
		Events::gameProcessEvent.Add(testNRG);
		Events::gameProcessEvent.Add(testSpeak);
	}

private:
	static void addSpeak() {
		AudioPath audioPath = { true, "SCRIPT", 22, 2 };
		while (!Speak::canAddSpeak()) { //may cost time
			Log::printInfo("waiting for speaker");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		Speak::addSpeak(FindPlayerPed(), audioPath, "Test very long audio to speak");
	}

	static void testSpeak() {
		if (cheat_pressed("speak")) {
			std::thread t(&addSpeak);
			t.detach();
		}
	}

	static void testCHS() {
		if (cheat_pressed("CHS")) {
			std::string text = Config::getTestSubtitle();
			Log::printInfo("call 4, trigger RenderFontBuffer");
			Subtitle::printSubtitle(text);
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