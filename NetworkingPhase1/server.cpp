#include "server.h"

int server::init(uint16_t port)
{
	uint8_t result = SHUTDOWN;
	int errorCheck = startup();
	LPWSADATA data;
	int errorCheck;
	std::cout << "Please Connect: ";
	std::cin >> errorCheck;
	std::cout << std::endl;

	if (0 != errorCheck)
		return result;
	SOCKET sockList = INVALID_SOCKET;
	sockList = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sockList) {
		result = SETUP_ERROR;
		return result;
	}
	struct sockaddr_in serverTCP = {};
	serverTCP.sin_family = AF_INET;
	serverTCP.sin_addr.s_addr = INADDR_ANY;
	serverTCP.sin_port = htons(port);
	if (0 != bind(sockList, (struct sockaddr*)&serverTCP, sizeof(serverTCP))) {
		result = BIND_ERROR;
		close(sockList);
		return result;
	}

	if (0 != listen(sockList, 5)) {
		result = SETUP_ERROR;
		close(sockList);
		return result;
	}
	SOCKET sockAccept = INVALID_SOCKET;
	struct sockaddr clientTCP = {};
	int clientTCPSize = sizeof(clientTCP);
	sockAccept = accept(sockList, &clientTCP, &clientTCPSize);
	if (INVALID_SOCKET == sockAccept) {
		result = CONNECT_ERROR;
		close(sockList);
		return result;
	}
	result = SUCCESS;
	sockServer = sockList;
	sockClient = sockAccept;
	return result;
}
int server::readMessage(char* buffer, int32_t size)
{
	int checkError = recv(sockClient, buffer, size, 0);
	if (checkError > size)
		return PARAMETER_ERROR;
	if (checkError == SOCKET_ERROR)
	{
		if (checkError == 0)
			return SHUTDOWN;
		else
			return DISCONNECT;
	}
	else
		return SUCCESS;
}

int server::sendMessage(char* data, int32_t length)
{
	if (0 > length || 255 < length)
		return PARAMETER_ERROR;
	int sendLegnth = send(sockClient, data, length, 0);
	if (sendLegnth == SOCKET_ERROR)
		return DISCONNECT;
	else if (sendLegnth == 0)
		return SHUTDOWN;
	return SUCCESS;

}
void server::stop()
{
	/*if (sockServer != INVALID_SOCKET)
	{
		shutdown(sockServer, SD_BOTH);
		closesocket(sockServer);
		sockServer = INVALID_SOCKET;
	}
	if (sockClient != INVALID_SOCKET)
	{
		shutdown(sockClient, SD_BOTH);
		closesocket(sockClient);
		sockClient = INVALID_SOCKET;
	}*/
	WSACleanup();
	shutdown();
	std::cout << "Exit\n";
}
