#pragma once
#include <cctype>

#include "plugin.h"
#include "CCheat.h"

using namespace plugin;

class CheatActivate {
public:
	static bool cheat_pressed(const char* usr_cheat) {
		std::string str(usr_cheat);
		transform(str.begin(), str.end(), str.begin(), ::toupper);
		std::reverse(str.begin(), str.end());
		if (!str.compare(0, str.size(), CCheat::m_CheatString, str.size())) {
			CCheat::m_CheatString[0] = '\0';
			return true;
		}
		return false;
	}

	static bool cheat_pressed(std::string str) {
		transform(str.begin(), str.end(), str.begin(), ::toupper);
		std::reverse(str.begin(), str.end());
		if (!str.compare(0, str.size(), CCheat::m_CheatString, str.size())) {
			CCheat::m_CheatString[0] = '\0';
			return true;
		}
		return false;
	}
};
