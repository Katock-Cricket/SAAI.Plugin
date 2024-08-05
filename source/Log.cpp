#include "Log.h"

std::mutex Log::log_mutex;
int Log::logLevel = 0;