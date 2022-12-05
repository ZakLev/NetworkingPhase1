#pragma once
#include <iostream>
#include "../platform.h"
#include "../definitions.h"
class server
{
private:

	SOCKET sockServer = INVALID_SOCKET;
	SOCKET sockClient = INVALID_SOCKET;

public:

	int init(uint16_t port);
	int readMessage(char* buffer, int32_t size);
	int sendMessage(char* data, int32_t length);
	void stop();
};

