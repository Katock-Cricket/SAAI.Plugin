#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>
#include <sstream>
#include <cctype>

#include "shared/Log.h"

enum JSONErr
{
	SUCCESS = 0,
	KEY_NOT_FOUND_OR_NOT_STRING = 1,
	KEY_NOT_FOUND_OR_NOT_ARRAY = 2,
	KEY_NOT_FOUND_OR_NOT_JSON = 3
};

class JSON;

typedef struct {
	JSONErr err;
	std::string str;
	std::vector<JSON> jsonArray;
	JSON* json;
}JSONRes;

class JSON {
public:

	JSON(const std::string& jsonString) {
		parse(jsonString);
	}

	JSON(const JSON& other) {
		data = copyData(other.data);
	}

	JSON() {}

	bool isEmpty() {
		return data.begin() == data.end();
	}

	std::string toString() const {
		std::ostringstream oss;
		toStringHelper(oss, *this);
		return oss.str();
	}

	bool addString(const std::string& key, const std::string& value) {
		data[key] = value;
		return true;
	}

	JSONRes getString(const std::string& key) const {
		JSONRes res = { SUCCESS , "", {}, nullptr };

		if (data.find(key) != data.end() && std::holds_alternative<std::string>(data.at(key))) {
			res.str = std::get<std::string>(data.at(key));
		}
		else {
			res.err = KEY_NOT_FOUND_OR_NOT_STRING;
		}
		return res;
	}

	bool addArray(const std::string& key, const std::vector<JSON>& array) {
		data[key] = array;
		return true;
	}

	JSONRes getArray(const std::string& key) const {
		JSONRes res = { SUCCESS , "", {}, nullptr };
		if (data.find(key) != data.end() && std::holds_alternative<std::vector<JSON>>(data.at(key))) {
			res.jsonArray = std::get<std::vector<JSON>>(data.at(key));
		}
		else {
			res.err = KEY_NOT_FOUND_OR_NOT_ARRAY;
		}
		return res;
	}

	bool addChild(const std::string& key, const JSON& child) {
		data[key] = child;
		return true;
	}

	JSONRes getChild(const std::string& key) const {
		JSONRes res = { SUCCESS , "", {}, nullptr };
		if (data.find(key) != data.end() && std::holds_alternative<JSON>(data.at(key))) {
			res.json = new JSON(std::get<JSON>(data.at(key)));
		}
		else {
			res.err = KEY_NOT_FOUND_OR_NOT_JSON;
		}
		return res;
	}

	unsigned int parse(const std::string& jsonString) {
		enum class State { EXPECT_KEY, IN_KEY, EXPECT_COLON, EXPECT_VALUE, IN_STRING, IN_ARRAY, IN_OBJECT, IN_NULL, IN_INT };

		State state = State::EXPECT_KEY;
		std::string key;
		std::string str;
		std::string number;
		std::vector<JSON> array;
		std::string nestedStr;
		JSON object;

		for (unsigned int i = 0; i < jsonString.length(); i++) {
			char c = jsonString[i];
			switch (state) {
			case State::EXPECT_KEY:
				if (c == '{') {
					state = State::IN_OBJECT;
				}
				else if (c == '"') {
					state = State::IN_KEY;
				}
				else if (c == '}') {
					return i;
				}
				break;
			case State::IN_KEY:
				if (c == '"') {
					state = State::EXPECT_COLON;
				}
				else {
					key += c;
				}
				break;
			case State::EXPECT_COLON:
				if (c == ':') {
					state = State::EXPECT_VALUE;
				}
				break;
			case State::EXPECT_VALUE:
				if (c == '"') {
					state = State::IN_STRING;
					str.clear();
				}
				else if (c == '[') {
					state = State::IN_ARRAY;
					nestedStr.clear();
					array.clear();
				}
				else if (c == '{') {
					state = State::EXPECT_KEY;
					nestedStr = jsonString.substr(i);
					JSON nestedJSON(nestedStr);
					i += JSON().parse(nestedStr);
					data[key] = nestedJSON;
					key.clear();
				}
				else if (c == 'n') {
					state = State::IN_NULL;
					str.clear();
				}
				else if (isdigit(c)) {
					state = State::IN_INT;
					number.clear();
					number += c;
				}
				else if (!std::isspace(c)) {
					Log::printError("Invalid JSON format : unexpected character: " + std::string(1, c));
					throw std::runtime_error("Invalid JSON format: unexpected character");
				}
				break;
			case State::IN_STRING:
				if (c == '"') {
					state = State::EXPECT_KEY;
					data[key] = str;
					key.clear();
				}
				else {
					str += c;
				}
				break;
			case State::IN_ARRAY:
				if (c == ']') {
					state = State::EXPECT_KEY;
					data[key] = array;
					key.clear();
				}
				else if (c == '{') {
					nestedStr = jsonString.substr(i);
					JSON nestedJSON(nestedStr);
					array.push_back(nestedJSON);
					i += JSON().parse(nestedStr);
				}
				break;
			case State::IN_OBJECT:
				if (c == '}') {
					state = State::EXPECT_KEY;
					data[key] = object;
					key.clear();
				}
				else if (c == '"') {
					state = State::IN_KEY;
				}
				break;
			case State::IN_NULL:
				if (c == ',' || c == '}') {
					str = "null";
					data[key] = str;
					key.clear();
					state = State::EXPECT_KEY;
				}
				break;
			case State::IN_INT:
				if (!isdigit(c)) {
					state = State::EXPECT_KEY;
					data[key] = std::int32_t(atoi(number.c_str()));
					key.clear();
				}
				else {
					number += c;
				}
				break;
			}
		}
		return 0;
	}

private:
	using JSONValue = std::variant<std::string, std::vector<JSON>, JSON, std::int32_t>;
	std::unordered_map<std::string, JSONValue> data;

	static void toStringHelper(std::ostringstream& oss, const JSON& json, int indentLevel = 0) {
		std::string indentation(indentLevel * 2, ' ');
		oss << "{\n";
		bool first = true;
		for (const auto& [key, value] : json.data) {
			if (!first) {
				oss << ",\n";
			}
			first = false;
			oss << indentation << "\"" << key << "\": ";
			if (std::holds_alternative<std::string>(value)) {
				oss << "\"" << std::get<std::string>(value) << "\"";
			}
			else if (std::holds_alternative<std::vector<JSON>>(value)) {
				oss << "[";
				bool firstInArray = true;
				for (const auto& item : std::get<std::vector<JSON>>(value)) {
					if (!firstInArray) {
						oss << ",\n";
					}
					firstInArray = false;
					toStringHelper(oss, item, indentLevel + 1);
				}
				oss << "]";
			}
			else if (std::holds_alternative<JSON>(value)) {
				toStringHelper(oss, std::get<JSON>(value), indentLevel + 1);
			}
		}
		oss << "\n" << indentation << "}";
	}

	std::unordered_map<std::string, JSONValue> copyData(const std::unordered_map<std::string, JSONValue>& otherData) const {
		std::unordered_map<std::string, JSONValue> newData;
		for (const auto& [key, value] : otherData) {
			newData[key] = copyJSONValue(value);
		}
		return newData;
	}

	JSONValue copyJSONValue(const JSONValue& value) const {
		return std::visit([](const auto& v) -> JSONValue {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, std::string>) {
				return v;
			}
			else if constexpr (std::is_same_v<T, std::vector<JSON>>) {
				return std::vector<JSON>(v);
			}
			else if constexpr (std::is_same_v<T, JSON>) {
				return JSON(v);
			}
			else if constexpr (std::is_same_v<T, std::int32_t>) {
				return v;
			}
			}, value);
	}
};
