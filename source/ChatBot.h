#pragma once
#include <curl/curl.h>
#include <sstream>
#include <string>

#include "CHud.h"

#include "Config.h"
#include "Subtitle.h"
#include "Log.h"
#include "JSON.h"

class ChatBot {
private:
	std::string url;
	std::string api_key;
	std::string model;
	std::vector<JSON> messages;
	bool initialized;

	struct MemoryStruct {
		char* memory;
		size_t size;
	};
	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		size_t realsize = size * nmemb;
		struct MemoryStruct* mem = (struct MemoryStruct*)userp;
		char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);

		if (ptr == NULL) {
			printf("realloc() failed\n");
			return 0;
		}

		mem->memory = ptr;
		memcpy(&(mem->memory[mem->size]), contents, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;

		return realsize;
	}
	
	void printJSONArray(std::vector<JSON> array) {
		Log::printInfo("[");
		for (JSON item : array) {
			Log::printInfo(item.toString());
			Log::printInfo(",");
		}
		Log::printInfo("]");
	}

	std::string autoDeleteChara(const std::string& answer) {
		std::string cnColon = "：";
		std::string enColon = ":";

		size_t posCn = answer.find(cnColon);
		size_t posEn = answer.find(enColon);

		size_t pos = min(posCn, posEn);

		if (pos == std::string::npos) {
			return answer;
		}
		std::string ret = answer.substr(pos + 1);
		Log::printInfo("Chatbot's answer contains colon, oringin ans: " + answer);
		Log::printInfo("auto delete colon: " + ret);
		return ret;
	}

	JSON send()
	{
		CURL* curl;
		CURLcode res;
		struct MemoryStruct chunk = { 0 };

		JSON json = JSON(model);
		json.addArray("messages", messages);

		JSON result = JSON();

		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();
		if (!curl) {
			return result;
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_slist* headers = NULL;
		headers = curl_slist_append(headers, api_key.c_str());
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		std::string jsonStr = json.toString();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonStr.size());
			
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			Log::printError("curl_easy_perform() failed" + std::string(curl_easy_strerror(res)));
			return result;
		}

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (chunk.memory == nullptr) {
			Log::printError("chunk == null");
			return result;
		}
		JSON response = JSON(std::string(chunk.memory));

		JSONRes errCheck = response.getChild("error");
		if (errCheck.err == 0) {
			JSON* err = errCheck.json;
			Log::printError(err->toString());
			return result;
		}
				 
		JSONRes jsonRes = response.getArray("choices");
		if (jsonRes.err != 0) {
			Log::printError("choices not found due to JSON err:" + std::to_string(jsonRes.err));
			return result;
		}
		std::vector<JSON> choices = jsonRes.jsonArray;
		jsonRes = choices[0].getChild("message");
		if (jsonRes.err != 0) {
			Log::printError("message not found due to JSON err:" + std::to_string(jsonRes.err));
			return result;
		}
		result = *(jsonRes.json);
		delete jsonRes.json;
		curl_easy_cleanup(curl);
		if (chunk.memory != NULL)
			free(chunk.memory);
		return result;
	}

public:
	ChatBot(std::string sys_prompt = "") {
		url = Config::getUrl();
		api_key = "Authorization: Bearer " + Config::getApiKey();
		model = "{\"model\": \"" + Config::getModel() + "\"}";
		initialized = false;
		if (sys_prompt != "") {
			ask((Config::getGlobalPrompt() + sys_prompt).c_str(), true);
			initialized = true;
		}
	}

	bool isInitialized() {
		return initialized;
	}

	std::string ask(std::string msg, bool set_sys_prompt = false) {
		JSON item = JSON();
		if (set_sys_prompt) {
			Log::printInfo("init chatbot");
			clear();
			item.addString("role", "system");
			item.addString("content", Config::getGlobalPrompt() + msg);
			messages.push_back(item);
			JSON item1 = JSON();
			item1.addString("role", "assistant");
			item1.addString("content", "OK");
			messages.push_back(item1);
			initialized = true;
			return "Init operation";
		}
		else {
			item.addString("role", "user");
			item.addString("content", msg);
		}
		if (messages.size() > Config::getMessagesSize()) {
			clear();
			JSON item0 = JSON();
			item0.addString("role", "system");
			item0.addString("content", Config::getGlobalPrompt() + msg);
			messages.push_back(item0);
			JSON item1 = JSON();
			item1.addString("role", "assistant");
			item1.addString("content", "OK");
			messages.push_back(item1);
		}
		messages.push_back(item);
		//Log::printInfo("msg generated, push msg");
		JSON res = send();
		//Log::printInfo("got msg");

		std::string answer;
		if (!res.isEmpty())
		{
			messages.push_back(res);
			JSONRes jsonRes = res.getString("content");
			if (jsonRes.err != 0) {
				Log::printError("answer is null due to JSON err: " + std::to_string(jsonRes.err));
				answer = "Error";
				messages.pop_back();
			}
			else {
				answer = jsonRes.str;
			}
			
		}
		else
		{
			messages.pop_back();
			answer = "Error";
		}
		//Log::printInfo("answer: " + answer);
		//Log::printInfo("messages:");
		//printJSONArray(messages);
		return autoDeleteChara(answer);
	}

	void clear() {
		std::vector<JSON>().swap(messages);
	}
};