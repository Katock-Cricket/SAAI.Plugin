#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class INIReader {
public:
	INIReader(const std::string& iniPath) {
		if (parseFile(iniPath)) {
			parseSuccess = true;
		}
		else {
			parseSuccess = false;
		}
	}

	std::string Get(const std::string& section, const std::string& key, const std::string& default_value) {
		std::string section_key = section + "." + key;
		if (values.find(section_key) != values.end()) {
			return values[section_key];
		}
		else {
			return default_value;
		}
	}

	int GetInteger(const std::string& section, const std::string& key, int default_value) {
		std::string value = Get(section, key, "");
		if (!value.empty()) {
			try {
				return std::stoi(value);
			}
			catch (std::invalid_argument&) {
				return default_value;
			}
		}
		else {
			return default_value;
		}
	}

	float GetFloat(const std::string& section, const std::string& key, float default_value) {
		std::string value = Get(section, key, "");
		if (!value.empty()) {
			try {
				return std::stof(value);
			}
			catch (std::invalid_argument&) {
				return default_value;
			}
		}
		else {
			return default_value;
		}
	}

	bool isParseSuccess() const {
		return parseSuccess;
	}

private:
	bool parseSuccess;
	std::map<std::string, std::string> values;

	bool parseFile(const std::string& iniPath) {
		std::ifstream file(iniPath);
		if (!file.is_open()) {
			return false;
		}

		std::string line, current_section;
		while (std::getline(file, line)) {
			// Remove comments
			size_t comment_pos = line.find_first_of("#");
			if (comment_pos != std::string::npos) {
				line.erase(comment_pos);
			}

			// Trim whitespace
			line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
			line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

			if (line.empty()) {
				continue;
			}

			// Section
			if (line.front() == '[' && line.back() == ']') {
				current_section = line.substr(1, line.size() - 2);
			}
			// Key-value pair
			else {
				size_t equals_pos = line.find('=');
				if (equals_pos != std::string::npos) {
					std::string key = line.substr(0, equals_pos);
					std::string value = line.substr(equals_pos + 1);

					// Trim whitespace from key and value
					key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
					key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
					value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
					value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

					values[current_section + "." + key] = value;
				}
			}
		}
		file.close();
		return true;
	}
};