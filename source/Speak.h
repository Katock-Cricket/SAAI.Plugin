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

	static bool canSpeak() {
		return !speakBuf.empty();
	}

	static SpeakTask getSpeakTask() {
		SpeakTask ret = speakBuf.front();
		speakBuf.pop();
		return ret;
	}

	static bool pedSpeak(CPed* ped, std::string pakName, int bankNumber, int wavNumber) {
		if (!ped->IsPointerValid()) {
			return false;
		}
		int trueBankNumber = getBankNumber(pakName, bankNumber);
		if (trueBankNumber == 0) {
			return false;
		}
		int soundID = calcSoundID(trueBankNumber, wavNumber);
		ped->SayScript(soundID, 0, 0, 0);
		return true;
	}

public:
	static void install() {
		Events::gameProcessEvent += [] {
			std::lock_guard<std::mutex> lock(speakMutex);
			if (canSpeak()) {
				SpeakTask speakTask = getSpeakTask();
				if (pedSpeak(speakTask.pedToSpeak, speakTask.audioToSpeak.pakName, speakTask.audioToSpeak.bankNumber, speakTask.audioToSpeak.wavNumber)) {
					Subtitle::printSubtitle(speakTask.contentToSpeak.c_str());
				}
			}
			};
	}

	static void addSpeak(CPed* ped, AudioPath audioPath, std::string content) {
		SpeakTask speakTask;
		speakTask.pedToSpeak = ped;
		speakTask.audioToSpeak = audioPath;
		speakTask.contentToSpeak = content;
		std::this_thread::sleep_for(std::chrono::milliseconds(Config::getReinstTime()));
		std::lock_guard<std::mutex> lock(speakMutex);
		speakBuf.push(speakTask);
		if (speakBuf.size() > 5) {
			speakBuf.pop();
		}
	}
};
