#include "AIMain.h"

std::vector<AIPed*> AIMain::AIPedPool;
std::queue<AIBeh*> AIMain::contentBuf;
std::queue<AIBeh*> AIMain::audioBuf;
std::vector<AIMain::Record> AIMain::history;
std::mutex AIMain::contentMutex;
std::mutex AIMain::audioMutex;
std::mutex AIMain::historyMutex;