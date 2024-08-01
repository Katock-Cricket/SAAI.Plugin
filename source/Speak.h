#pragma once
#include<map>
#include<queue>

#include "CPed.h"
#include "SVCClient.h"
#include "Subtitle.h"
#include "utils.h"

typedef struct {
	CPed* pedToSpeak;
	AudioPath audioToSpeak;
	std::string contentToSpeak;
} SpeakTask;

class Speak {
private:
	static std::mutex speakMutex;
	static std::map<std::string, int> pakSize;
	static std::queue<SpeakTask> speakBuf;
	static CPed* pedSpeaking;
	static std::mutex lockMutex;
	static bool speakLock;
	 
	static int getBankNumber(std::string pakName, unsigned bankNumber) {
		std::map<std::string, int>::iterator it;
		int offset = 0;
		for (it = pakSize.begin(); it != pakSize.end(); offset += it->second, it++) {
			if (it->first == pakName) {
				return offset + bankNumber;
			}
		}
		return 0;
	}

	static int calcSoundID(int trueBankNumber, int wavNumber) {
		return (trueBankNumber - 1) * 200 + 2000 + (wavNumber - 1);
	}

	static SpeakTask getSpeakTask() {
		SpeakTask ret = speakBuf.front();
		speakBuf.pop();
		return ret;
	}

	static bool pedSpeak(CPed* ped, std::string pakName, int bankNumber, int wavNumber) {
		if (ped == nullptr || !ped->IsPointerValid()) {
			return false;
		}
		int trueBankNumber = getBankNumber(pakName, bankNumber);
		if (trueBankNumber == 0) {
			return false;
		}
		int soundID = calcSoundID(trueBankNumber, wavNumber);
		ped->EnablePedSpeech();
		ped->SayScript(soundID, 0, 0, 0);
		pedSpeaking = ped;
		std::thread t(&autoForceStopTalk, ped);
		t.detach();
		std::lock_guard<std::mutex> lock(lockMutex);
		speakLock = true;
		Log::printInfo("Ped is going to say audio: pak:" + pakName + " bank:" + std::to_string(bankNumber) + " wav:" + std::to_string(wavNumber));
		return true;
	}

	static bool canSpeak() {
		// no ped is speaking and have speak task
		if (pedSpeaking == nullptr || !IsPedPointerValid(pedSpeaking)) {
			std::lock_guard<std::mutex> lock(lockMutex);
			speakLock = false;;
			return !speakBuf.empty();
		}
		if (pedSpeaking->GetPedTalking()) {
			return false;
		}
		std::lock_guard<std::mutex> lock(lockMutex);
		speakLock = false;
		return !speakBuf.empty();
	}

	static void processSpeak() {
		std::lock_guard<std::mutex> lock(speakMutex);
		if (canSpeak()) {
			//Log::printInfo("get speak task");
			SpeakTask speakTask = getSpeakTask();
			if (pedSpeak(speakTask.pedToSpeak, speakTask.audioToSpeak.pakName, speakTask.audioToSpeak.bankNumber, speakTask.audioToSpeak.wavNumber)) {
				Subtitle::printSubtitle(speakTask.contentToSpeak, speakTask.pedToSpeak);
			}
		}
	}

	static bool isOccupied(CPed* ped) {
		if (pedSpeaking == nullptr || !IsPedPointerValid(pedSpeaking)) {
			return false;
		}
		std::lock_guard<std::mutex> lock(lockMutex);
		return speakLock && ped == pedSpeaking;
	}

	static short __fastcall lockPriorityWhenSay(CPed* ped, int, unsigned short arg0, unsigned int arg1, float arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5) {
		if (isOccupied(ped)) {
			//Log::printInfo("Maintain AI speak, when ped want to Say");
			return -1;
		}
		//Log::printInfo("Allow say");
		//Log::printInfo("ped ptr: " + std::to_string(IsPedPointerValid(ped)));
		//Log::printInfo("arg0: " + std::to_string(arg0));
		//Log::printInfo("arg1: " + std::to_string(arg1));
		//Log::printInfo("arg2: " + std::to_string(arg2));
		//Log::printInfo("arg3: " + std::to_string(arg3));
		//Log::printInfo("arg4: " + std::to_string(arg4));
		//Log::printInfo("arg5: " + std::to_string(arg5));
		if (!IsPedPointerValid(ped)) {
			Log::printError("ped is null when say");
			return -1;
		}
		if (arg0) {
			return plugin::CallMethodAndReturn<short, 0x4E6550, CAEPedSpeechAudioEntity*, int, unsigned short, unsigned int, float, unsigned char, unsigned char, unsigned char>
				(&(ped->m_pedSpeech), 0x34, arg0, arg1, arg2, arg3, arg4, arg5);
		}
		return -1;
	}

	static void __fastcall lockPriorityWhenSayScript(CPed* ped, int, int arg0, unsigned char arg1, unsigned char arg2, unsigned char arg3) {
		if (isOccupied(ped)) {
			Log::printInfo("Maintain AI speak, when ped want to SayScript");
			return;
		}
		//Log::printInfo("Allow say script");
		//Log::printInfo("ped ptr: " + std::to_string(IsPedPointerValid(ped)));
		//Log::printInfo("arg0: " + std::to_string(arg0));
		//Log::printInfo("arg1: " + std::to_string(arg1));
		//Log::printInfo("arg2: " + std::to_string(arg2));
		//Log::printInfo("arg3: " + std::to_string(arg3));
		if (!IsPedPointerValid(ped)) {
			Log::printError("ped is null when say script");
			return;
		}
		plugin::CallMethod<0x4E4F70, CAEPedSpeechAudioEntity*, int, int, unsigned char, unsigned char, unsigned char>
			(&(ped->m_pedSpeech), 0x35, arg0, 100, 0, 0);
	}

	static void autoForceStopTalk(CPed* ped) { //when a script is too long, npc won't stop talking, force to stop
		std::this_thread::sleep_for(std::chrono::milliseconds(6000));
		if (!IsPedPointerValid(ped) || pedSpeaking == nullptr) {
			Log::printError("pedSpeaking secs ago is null");
			return;
		}
		if (ped == pedSpeaking && ped->GetPedTalking()) { // still this ped speaking
			Log::printInfo("force mute pedSpeaking secs ago");
			ped->DisablePedSpeechForScriptSpeech(1);
		}
	}

public:
	static void install() { // install when init
		Events::gameProcessEvent.Add(processSpeak);
		injector::MakeJMP(0x5EFFE0, lockPriorityWhenSay);
		injector::MakeJMP(0x5EFFB0, lockPriorityWhenSayScript);
	}

	static void uninstall() { // uninstall when shutdown
		std::lock_guard<std::mutex> lock0(speakMutex);
		std::queue<SpeakTask>().swap(speakBuf);
		pedSpeaking = nullptr;
		std::lock_guard<std::mutex> lock1(lockMutex);
		speakLock = false;
	}

	static bool canAddSpeak() {
		std::lock_guard<std::mutex> lock(speakMutex);
		return speakBuf.size() < Config::getSpeakbufferSize();
	}

	static void addSpeak(CPed* ped, AudioPath audioPath, std::string content) {
		SpeakTask speakTask;
		speakTask.pedToSpeak = ped;
		speakTask.audioToSpeak = audioPath;
		speakTask.contentToSpeak = content;
		std::this_thread::sleep_for(std::chrono::milliseconds(Config::getReinstTime()));
		std::lock_guard<std::mutex> lock(speakMutex);
		speakBuf.push(speakTask);
	}

	static void autoAddSpeak(CPed* ped, AudioPath audioPath, std::string content) {
		while (!canAddSpeak()) {
			//Log::printInfo("waiting for speaker");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		addSpeak(ped, audioPath, content);
	}
};
