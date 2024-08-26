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

//bellow are shared resources
static std::mutex contentMutex;
static std::mutex audioMutex;
static std::mutex historyMutex;
static std::mutex chatMutex;

static std::queue<AIBeh*> contentBuf;
static std::vector<AIBeh*> audioBuf; // allow multiple aiBeh in progress
static std::vector<Record> history;
static bool isChating;

static void addAIPed(CPed* ped, std::string name) {
	AIPed* aiPed = new AIPed(ped, name);
	AIPedPool.push_back(aiPed);
}

static bool noValidRecord() { // need mutex
	if (history.empty()) {
		return true;
	}
	for (auto it = history.begin(); it != history.end(); ++it) {
		if (!it->content.empty()) {
			return false;
		}
	}
	return true;
}

static Record* lastRecord() { // need mutex
	for (auto it = history.rbegin(); it != history.rend(); ++it) {
		if (!it->content.empty()) {
			return &(*it);
		}
	}
	return nullptr;
}

static Record* nextEmptyRecord() { // need mutex
	for (auto it = history.begin(); it != history.end(); ++it) {
		if (it->content.empty()) {
			return &(*(it));
		}
	}
	return nullptr;
}