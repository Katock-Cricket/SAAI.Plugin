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
		ped->SayScript(soundID, 0, 0, 0);
		pedSpeaking = ped;
		Log::printInfo("Ped is going to say audio: pak:" + pakName + " bank:" + std::to_string(bankNumber) + " wav:" + std::to_string(wavNumber));
		return true;
	}

	static bool canSpeak() {
		// no ped is speaking and have speak task
		if (pedSpeaking == nullptr || !IsPedPointerValid(pedSpeaking)) {
			return !speakBuf.empty();
		}
		if (pedSpeaking->GetPedTalking()) {
			return false;
		}
		return !speakBuf.empty();
	}

	static void processSpeak() {
		std::lock_guard<std::mutex> lock(speakMutex);
		if (canSpeak()) {
			Log::printInfo("get speak task");
			SpeakTask speakTask = getSpeakTask();
			if (pedSpeak(speakTask.pedToSpeak, speakTask.audioToSpeak.pakName, speakTask.audioToSpeak.bankNumber, speakTask.audioToSpeak.wavNumber)) {
				Subtitle::printSubtitle(speakTask.contentToSpeak, speakTask.pedToSpeak);
			}
		}
	}

public:
	static void install() {
		Events::gameProcessEvent.Add(processSpeak);
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
