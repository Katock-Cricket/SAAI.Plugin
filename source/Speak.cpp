#include "Speak.h"

std::map<std::string, int> Speak::pakSize = {
	{"SCRIPT", 218},
	{"SPC_EA", 46},
	{"SPC_FA", 18},
	{"SPC_GA", 209},
	{"SPC_NA", 52},
	{"SPC_PA", 20}
};
std::queue<SpeakTask> Speak::speakBuf;
std::mutex Speak::bufMutex;
std::mutex Speak::pedMutex;
CPed* Speak::pedSpeaking = nullptr;
bool Speak::speakLock = false;
std::mutex Speak::lockMutex;
unsigned int Speak::timer;

