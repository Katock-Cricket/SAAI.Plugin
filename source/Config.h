#pragma once
#include <iostream>

#include "INIReader.h"

#include "Log.h"

class Config {
private:
    static std::string url;
    static std::string api_key;
    static std::string model;
    static int svc_timeout;
    static int reinst_time;
    static std::string global_prompt;
    static std::map<std::string, std::string> sys_prompt;
    static int content_buffer_size;
    static int audio_buffer_size;
    static int messages_size;
    static std::string testSubtitle;


public:
    static std::string getUrl() {
        return url;
    }

    static std::string getApiKey() {
        return api_key;
    }

    static std::string getModel() {
        return model;
    }

    static int getSVCTimeout() {
        return svc_timeout;
    }

    static int getReinstTime() {
        return reinst_time;
    }

    static std::string getGlobalPrompt() {
        return global_prompt;
    }

    static std::string getSysPrompt(std::string name) {
        if (sys_prompt.find(name) == sys_prompt.end()) {
            return "";
        }
        return sys_prompt[name];
    }

    static unsigned int getContentBufSize() {
        return content_buffer_size;
    }

    static unsigned int getAudioBufSize() {
        return audio_buffer_size;
    }    
    
    static unsigned int getMessagesSize() {
        return messages_size;
    }

    static std::string getTestSubtitle() {
        return testSubtitle;
    }

    static bool loadFromINI(const char* INIPath) {
        INIReader reader(INIPath);

        if (!reader.isParseSuccess()) {
            return false;
        }

        try {
            url = reader.Get("ChatGPT", "url", "");
            api_key = reader.Get("ChatGPT", "api_key", "");
            model = reader.Get("ChatGPT", "model", "");
            svc_timeout = reader.GetInteger("TTS-SVC", "timeout", 5);
            reinst_time = reader.GetInteger("TTS-SVC", "reinstall", 1000);
            global_prompt = reader.Get("Personality", "global_prompt", "");
            for (auto& t : sys_prompt) {
                t.second = reader.Get("Personality", t.first, "");
            }
            content_buffer_size = reader.GetInteger("Performance", "content_buffer_size", 3);
            audio_buffer_size = reader.GetInteger("Performance", "audio_buffer_size", 3);
            messages_size = reader.GetInteger("Performance", "messages_size", 5);
            testSubtitle = reader.Get("CHS", "test_subtitle", "");
        }
        catch (std::exception& e) {
            Log::printError(e.what());
            return false;
        }
        Log::printInfo("Url: " + url, "../SAAI.log");
        Log::printInfo("APIKEY: " + api_key, "../SAAI.log");
        Log::printInfo("model: " + model, "../SAAI.log");
        Log::printInfo("svc_timeout: " + std::to_string(svc_timeout), "../SAAI.log"); 
        Log::printInfo("reinst_time: " + std::to_string(reinst_time), "../SAAI.log");
        Log::printInfo(global_prompt, "../SAAI.log");
        for (auto& t : sys_prompt) {
            Log::printInfo(t.first + " " + t.second, "../SAAI.log");
        }
        Log::printInfo("content_buffer_size: " + std::to_string(content_buffer_size), "../SAAI.log");
        Log::printInfo("audio_buffer_size: " + std::to_string(audio_buffer_size), "../SAAI.log");
        Log::printInfo("messages_size: " + std::to_string(messages_size), "../SAAI.log");
        Log::printInfo("test_subtitle: " + testSubtitle, "../SAAI.log");
        return true;
    }
};