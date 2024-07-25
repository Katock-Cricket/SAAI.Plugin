#pragma once
#include "plugin.h"
#include "CTimer.h"

using namespace plugin;

class KeyActivate {
private:
	static unsigned int keyPressTime;

public:
	static bool key_pressed(unsigned int key, unsigned int dur = 500) {
		if (KeyPressed(key) && CTimer::m_snTimeInMilliseconds - keyPressTime > dur) {
			keyPressTime = CTimer::m_snTimeInMilliseconds;
			return true;
		}
		return false;
	}
};