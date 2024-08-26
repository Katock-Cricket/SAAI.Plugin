#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

#include "config/Config.h"

class ThreadPool {
private:
	static inline std::vector<std::thread> workers;
	static inline std::queue<std::function<void()>> tasks;
	static inline std::mutex queueMutex;
	static inline std::condition_variable condition;
	static inline bool stop = false;
	static inline bool installed = false;
	static inline int workingCount = 0;

public:
	static void install() {
		if (installed) return;

		size_t numThreads = Config::getNumWorkers();
		for (size_t i = 0; i < numThreads; ++i) {
			workers.emplace_back([] {
				while (true) {
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(queueMutex);
						condition.wait(lock, [] { return !tasks.empty() || stop; });
						if (stop && tasks.empty()) return;
						task = std::move(tasks.front());
						tasks.pop();
						++workingCount;
					}
					task();
					{
						std::unique_lock<std::mutex> lock(queueMutex);
						--workingCount;
					}
				}
				});
		}
		installed = true;
	}

	static void uninstall() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
		workers.clear();
		stop = false;
	}

	static int getWorkingNum() {
		std::unique_lock<std::mutex> lock(queueMutex);
		return workingCount;
	}

	template<class F, class... Args>
	static void start(F&& f, Args&&... args) {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			tasks.emplace([f, args...]() {
				f(args...);
				});
		}
		condition.notify_one();
	}
};
