#pragma once
#include <thread>
#include <mutex>
#include <cstring>

#include "plugin.h"
#include "CFont.h"
#include "Config.h"

using namespace plugin;

class Subtitle {
private:
	static std::mutex subtitleMutex;
	static bool activate;
	static char content[512];
	static CPed* pedSpeaking;

	static bool isSpeaking() {
		if (pedSpeaking == nullptr || !IsPedPointerValid(pedSpeaking)) {
			return false;
		}
		if (pedSpeaking->GetPedTalking()) {
			return true;
		}
		return false;
	}

	static void autoDeactivate(std::string contentPre, int last) {
		std::this_thread::sleep_for(std::chrono::seconds(last));
		std::lock_guard<std::mutex> lock(subtitleMutex);
		if (std::string(content) == contentPre) {
			activate = false;
		}
	}

	static int calcDuring(const char* sentence) {
		bool cn = Config::getCN();

		if (sentence == nullptr || strlen(sentence) == 0) {
			return 0;
		}

		if (cn) {
			const double averageCharDuration = 0.3;
			int charCount = 0;

			const unsigned char* ptr = reinterpret_cast<const unsigned char*>(sentence);
			while (*ptr != '\0') {
				charCount++;
				ptr += 3;
			}

			return static_cast<int>(charCount * averageCharDuration + 0.5);
		}
		else {
			const double averageWordDuration = 0.5;
			int wordCount = 0;

			const char* delimiters = " ";
			char* tempSentence = new char[strlen(sentence) + 1];
			strcpy(tempSentence, sentence);

			char* token = strtok(tempSentence, delimiters);
			while (token != nullptr) {
				wordCount++;
				token = strtok(nullptr, delimiters);
			}

			delete[] tempSentence;
			return static_cast<int>(wordCount * averageWordDuration + 0.5);
		}
	}

	static void pedSpeakSubtitle() {
		std::lock_guard<std::mutex> lock(subtitleMutex);
		if (isSpeaking()) {
			CFont::SetProportional(true);
			CFont::SetBackground(false, false);
			CFont::SetScale(2.0f, 2.0f);
			CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
			CFont::SetCentreSize(SCREEN_WIDTH * 5 / 6);
			CFont::SetColor({ 255, 255, 255 });
			CFont::SetEdge(1);
			CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
			CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 6, content);
		}
		else {
			pedSpeaking = nullptr;
		}
	}

	static void otherSubtitle() {
		std::lock_guard<std::mutex> lock(subtitleMutex);
		if (activate) {
			CFont::SetProportional(true);
			CFont::SetBackground(false, false);
			CFont::SetScale(2.0f, 2.0f);
			CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
			CFont::SetCentreSize(SCREEN_WIDTH * 5 / 6);
			CFont::SetColor({ 255, 255, 255 });
			CFont::SetEdge(1);
			CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
			CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 6, content);
		}
	}

	static void pipeline() {
		//Log::printInfo("---------------Speak Pipeline-----------------");
		pedSpeakSubtitle();
		//Log::printInfo("pedSpeakSubtitle");
		otherSubtitle();
		//Log::printInfo("otherSubtitle");

	}

public:

	Subtitle() { }

	static void install() { // install when init
		Events::drawingEvent.Add(pedSpeakSubtitle);
		Events::drawingEvent.Add(otherSubtitle);
	}

	static void uninstall() { // uninstall when shutdown
		std::lock_guard<std::mutex> lock(subtitleMutex);
		activate = false;
		pedSpeaking = nullptr;
	}

	static void printSubtitle(std::string usr_content, CPed* ped = nullptr) {
		std::lock_guard<std::mutex> lock(subtitleMutex);
		std::strncpy(content, usr_content.c_str(), sizeof(content) - 1);
		content[sizeof(content) - 1] = '\0';
		if (ped == nullptr) {
			int last = calcDuring(content);
			std::thread t(&autoDeactivate, std::string(content), last);
			t.detach();
			activate = true;
		}
		else {
			pedSpeaking = ped;
		}
	}
};
