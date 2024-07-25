#include "Subtitle.h"

bool Subtitle::activate = false;
char Subtitle::content[512] = "";
int Subtitle::last = 2;
std::mutex Subtitle::subtitleMutex;