#include "SVCClient.h"

std::mutex SVCClient::wsaMutex;
WSADATA SVCClient::wsaData;
struct sockaddr_in SVCClient::serv_addr;
bool SVCClient::alive = true;
std::mutex SVCClient::aliveMutex;
