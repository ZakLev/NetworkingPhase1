// NetworkingPhase1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "../definitions.h"
#include "../platform.h"
int main()
{
    
	std::cout << "Waiting For Connection\n";
	 std::string ip = "127.0.0.1";			// Server IP Address
	int port = 3333;
    /*WSAData data;
    WORD ver = MAKEWORD(2, 2);*/
	int errorCheck = startup();//WSAStartup(ver, &data);
    if (errorCheck != 0)
    {
        std::cerr << "\nCan't start Winsock, Exit" << std::endl;
        return 0;
    }
	// Create socket
	SOCKET lis = socket(AF_INET, SOCK_STREAM, 0);
	if (lis == INVALID_SOCKET)
	{
		std::cerr << "\nCan't create socket, Exit" << std::endl;
		return 0;
	}

	//Bind
	sockaddr_in hin;
	hin.sin_family = AF_INET;
	hin.sin_port = htons(port);
	//bin.sin_addr.S_un.S_addr = INADDR_ANY;// Inet_Poton instead
	inet_pton(AF_INET, ip.c_str(), &hin.sin_addr);
	bind(lis, (sockaddr*)&hin, sizeof(hin));

	// listening
	listen(lis, SOMAXCONN);
	
	//wait
	sockaddr_in client;
	int clientSize = sizeof(client);
	
	SOCKET clientSock = accept(lis, (sockaddr*)&client, &clientSize);
	
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << "Connected on Port: " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "Connected on Port: " << ntohs(client.sin_port) << std::endl;
	}
	closesocket(lis);

	char buff[4096];// = { 0, };
	while (true)
	{
		ZeroMemory(buff, 4096);
		int bytes = recv(clientSock, buff, 4096, 0);
		if (bytes == SOCKET_ERROR)
		{
			std::cerr << "\nError in recV, Exit" << std::endl;
			break;
		}
		if (bytes == 0)
		{
			std::cout << "\nDisconnected" << std::endl;
			break;
		}
		std::cout << "Client(" << host << "): " << std::string(buff, 0, bytes) << std::endl;

		send(clientSock, buff, bytes + 1, 0);
	}

	// Gracefully close down everything
	closesocket(clientSock);
	WSACleanup();
	system("pause");
	//return 0;
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
