#pragma once

#include "AIBeh.h"
#include "Config.h"

class Record {
public:
	AIPed* aiPed;
	std::string name;
	std::string content;
	Record(AIPed* aiPed, std::string name, std::string content) {
		this->aiPed = aiPed;
		this->name = name;
		this->content = content;
	}
	std::string toString() {
		return name + ": " + content;
	}
};

static std::vector<AIPed*> AIPedPool; // use only in funcs-between-frames(sync), no need for mutex
static std::mutex contentMutex;
static std::mutex audioMutex;
static std::mutex historyMutex;
static std::queue<AIBeh*> contentBuf;
static std::queue<AIBeh*> audioBuf;
static std::vector<Record> history;

static void addAIPed(CPed* ped, std::string name) {
	AIPed* aiPed = new AIPed(ped, name);
	AIPedPool.push_back(aiPed);
}
