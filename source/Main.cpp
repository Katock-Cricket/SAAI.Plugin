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
	static void installPipeline() {
		PedSpawner::install();
		Subtitle::install();
		Speak::install();
		UnitTest::install();
		AIMain::install();
	}

	static void uninstPipeline() {
		Log::printInfo("uninst");
		Subtitle::uninstall();
		Speak::uninstall();
		AIMain::uninstall();
	}

public:
	SAAI() {
		Log::install();
		if (!Config::loadFromINI("SAAI.ini")) {
			return;
		}
		std::thread t(&system, ".\\SAAI.Server\\env\\python.exe .\\SAAI.Server\\server.py");
		t.detach();
		if (!SVCClient::init_socket()) {
			return;
		}
		installPipeline();
		Events::shutdownRwEvent.Add(uninstPipeline);
	} 
} SAAI;
