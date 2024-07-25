#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include "CHud.h"
#include "utils.h"
#include "Config.h"

class SVCClient {
private:
	static WSADATA wsaData;
	static std::mutex wsaMutex;
	SOCKET sock;
	static struct sockaddr_in serv_addr;

	AudioPath decodeAudioPath(const std::string& input) {
		AudioPath audioPath = { false, "", 0, 0 };
		std::vector<std::string> parts;
		std::istringstream iss(input);
		std::string part;

		while (std::getline(iss, part, ';')) {
			parts.push_back(part);
		}
		if (parts.size() == 3) {
			audioPath.valid = true;
			audioPath.pakName = parts[0];
			audioPath.bankNumber = std::stoi(parts[1]);
			audioPath.wavNumber = std::stoi(parts[2]);
		}
		else {
			std::cerr << "Input string does not have the correct format." << std::endl;
		}

		return audioPath;
	}

public:
	SVCClient() {
	}

	static bool init_socket() {
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			Log::printError("WSAStartup failed!!!!!!!!!!!!");
			return false;
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(65432);
		inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

		return true;
	}

	static bool close_socket() {
		WSACleanup();
	}

	bool init_client() {
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			Log::printError("Socket creation error!!!!!!!!!!!!");
			return false;
		}

		auto start_time = std::chrono::steady_clock::now();
		const std::chrono::seconds timeout(Config::getSVCTimeout());
		std::lock_guard<std::mutex> lock(wsaMutex);
		while (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
			Log::printError("Connetion Failed!!!!!!!!!!!!");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto current_time = std::chrono::steady_clock::now();
			if (current_time - start_time >= timeout) {
				Log::printError("Connetion Timeout!!!!!!!!!!!!");
				return false;
			}
		}
		return true;
	}

	AudioPath request_audio(std::string content, std::string speaker) {
		AudioPath audioPath = { false, "", 0, 0 };
		if (!init_client()) {
			return audioPath;
		}

		std::string message = content + ";" + speaker;
		send(sock, message.c_str(), message.length(), 0);

		char buffer[1024] = { 0 };
		std::string res;
		while (true) {
			int valread = recv(sock, buffer, 1024, 0);
			if (valread > 0) {
				res = std::string(buffer);
				break;
			}
		}
		audioPath = decodeAudioPath(res);
		closesocket(sock);
		return audioPath;
	}
};
