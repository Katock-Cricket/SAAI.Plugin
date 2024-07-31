#include "Config.h"

std::string Config::url = "";
std::string Config::api_key = "";
std::string Config::model = "";
bool Config::cn = true;
int Config::svc_timeout = 5;
int Config::reinst_time = 1000;
std::string Config::global_prompt = "";
std::string Config::meet_prompt = "";
std::map<std::string, std::string> Config::sys_prompt = {
	{"carl", ""},
	{"ryder", ""},
	{"sweet", ""},
	{"smoke", ""}
};
int Config::content_buffer_size = 3;
int Config::audio_buffer_size = 5;
int Config::messages_size = 5;
int Config::speak_buffer_size = 5;
std::string Config::testSubtitle = "";