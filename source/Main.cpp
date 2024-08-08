#include <cstdlib>
#include <thread>

#include "PedSpawner.h"
#include "Subtitle.h"
#include "Speak.h"
#include "Config.h"
#include "UnitTest.h"
#include "AIMain.h"
#include "Log.h"
#include "ThreadPool.h"

HANDLE StartProcess(const char* commandLine) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_MINIMIZE;

	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcessA(NULL,
		(LPSTR)commandLine,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		) {
		return NULL;
	}
	return pi.hProcess;
}

void TerminateProcess(HANDLE processHandle) {
	TerminateProcess(processHandle, 0);
	CloseHandle(processHandle);
}

class SAAI {
private:
	static inline HANDLE processHandle;

	static void installPipeline() {
		processHandle = StartProcess(".\\SAAI.Server\\env\\python.exe .\\SAAI.Server\\server.py");
		ThreadPool::install();
		PedSpawner::install();
		Subtitle::install();
		Speak::install();
		//UnitTest::install();
		ChatController::install();
		AIMain::install();
	}

	static void uninstPipeline() {
		SVCClient::uninstall();
		ThreadPool::uninstall();
		Subtitle::uninstall();
		Speak::uninstall();
		AIMain::uninstall();
		TerminateProcess(processHandle);
		Log::printInfo("Uninst SAAI");
	}

public:
	SAAI() {
		Log::install();
		if (!Config::install("SAAI.ini")) {
			return;
		}

		Events::initRwEvent.Add(installPipeline);

		if (!SVCClient::install()) {
			return;
		}

		Events::shutdownRwEvent.Add(uninstPipeline);
	}
} SAAI;