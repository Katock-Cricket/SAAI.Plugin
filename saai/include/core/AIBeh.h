#pragma once
#include "AI.h"

class AIBeh {
private:
	CPed* self;
	std::string context;
	std::string content;
	bool working;
    bool isFirstOfFreeChat;

public:
	AIBeh() {
		self = nullptr;
		this->working = false;
        this->isFirstOfFreeChat = false;
	}

	AIBeh(CPed* self) {
		this->self = self;
		this->context = "";
		this->content = "";
		this->working = false;
        this->isFirstOfFreeChat = false;
	}

	CPed* getPed() {
		return self;
	}

	std::string getContext() {
		return context;
	}

	void setContext(std::string context) {
		this->context = context;
	}

	std::string getContent() {
		return content;
	}

	void setContent(std::string content) {
		this->content = content;
	}

	bool isWorking() {
		return working;
	}

	void start() {
		working = true;
	}

	void finish() {
		working = false;
	}

    void setIsFirstOfFreeChat(bool isFirst) {
        isFirstOfFreeChat = isFirst;
    }

    bool getIsFirstOfFreeChat() {
        return isFirstOfFreeChat;
    }

    inline bool operator==(const AIBeh &aiBeh) const {
        return this->content == aiBeh.content;
    }
};