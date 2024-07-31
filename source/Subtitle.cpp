#include "Subtitle.h"

bool Subtitle::activate = false;
char Subtitle::content[512] = "";
CPed* Subtitle::pedSpeaking = nullptr;
std::mutex Subtitle::subtitleMutex;