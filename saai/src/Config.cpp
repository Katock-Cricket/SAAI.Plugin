#include "Config.h"

std::string Config::cheat_free_chat;
std::map<std::string, std::string> Config::cheat_npc = {
	{"Ryder", "ryder"},
	{"Sweet", "sweet"},
	{"Smoke", "smoke"}
};
std::string Config::url = "";
std::string Config::api_key = "";
std::string Config::model = "";
bool Config::cn = true;
int Config::svc_timeout = 5;
int Config::reinst_time = 1000;
std::string Config::global_prompt = "";
std::string Config::meet_prompt = "";
std::map<std::string, std::string> Config::sys_prompt = {
	{"Carl", ""},
	{"Ryder", ""},
	{"Sweet", ""},
	{"Smoke", ""}
};
int Config::content_buffer_size = 3;
int Config::audio_buffer_size = 5;
int Config::speak_buffer_size = 5;
int Config::chat_round = 5;
int Config::speak_timeout = 6000;
int Config::log_level = 0;
int Config::num_workers = 10;
int Config::free_wait_time = 0;