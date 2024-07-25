#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>

class Log {
private:
    static std::mutex log_mutex;

public:
    static void printInfo(const std::string& info, const char* file = "SAAI.log") {
        std::lock_guard<std::mutex> guard(log_mutex);
        std::ofstream logfile(file, std::ios_base::app);
        if (logfile.is_open()) {
            logfile << "[Info] " << info << std::endl;
        }
    }

    static void printInfo(const char* info, const char* file = "SAAI.log") {
        std::lock_guard<std::mutex> guard(log_mutex);
        std::ofstream logfile(file, std::ios_base::app);
        if (logfile.is_open()) {
            logfile << "[Info] " << info << std::endl;
        }
    }

    static void printError(std::string info, const char* file = "SAAI.log") {
        std::lock_guard<std::mutex> guard(log_mutex);
        std::ofstream logfile(file, std::ios_base::app);
        if (logfile.is_open()) {
            logfile << "[Error] " << info << std::endl;
        }
    }

    static void printError(const char* info, const char* file = "SAAI.log") {
        std::lock_guard<std::mutex> guard(log_mutex);
        std::ofstream logfile(file, std::ios_base::app);
        if (logfile.is_open()) {
            logfile << "[Error] " << info << std::endl;
        }
    }

    static void install() {
        std::lock_guard<std::mutex> guard(log_mutex);

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time);

        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

        std::ofstream logfile("../SAAI.log", std::ios::trunc);
        if (logfile.is_open()) {
            logfile << "Log initialized at: " << oss.str() << std::endl;
        }
        else {
            std::cerr << "Failed to open SAAI.log" << std::endl;
        }
    }
};

