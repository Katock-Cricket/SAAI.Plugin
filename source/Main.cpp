#include <cstdlib> 
#include <thread>

#include "PedSpawner.h"
#include "Subtitle.h"
#include "Speak.h"
#include "Config.h"
#include "UnitTest.h"
#include "AIMain.h"
#include "Log.h"

class SAAI {
private:
	PedSpawner pedSpawner;
	Subtitle subtitle;
	UnitTest unitTest;
public:
	SAAI() {
		Log::install();
		if (!Config::loadFromINI("SAAI.ini")) {
			return;
		}
		std::thread t(&system, ".\\SAAI.Server\\env\\python.exe .\\SAAI.Server\\server.py");
		t.detach();
		pedSpawner.install();
		subtitle.install();
		unitTest.install();
		Speak::install();
		if (SVCClient::init_socket()) {
			AIMain::install();
		}
	} 
} SAAI;
