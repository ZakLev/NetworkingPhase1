// NetworkingPhase2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "../definitions.h"
#include "../platform.h"
#pragma comment(lib,"WS2_32")//necessary for linkers

int main()
{
	////Get IP and PORT
	std::string ip, holdPort;
	int port;
	//std::cout << "Enter the ip: \n";
	//std::getline(std::cin, ip);
	//std::cout << std::endl;
	//std::cout << "Enter the Port: \n";
	//std::getline(std::cin,holdPort );
	//port = stoi(holdPort);
	//std::cout << std::endl;
	
	

	ip = "127.0.0.1";			// IP of server for testing
	port = 3333; // port of server for testing
	/*WSAData data;
	WORD ver = MAKEWORD(2, 2);*/
	int errorCheck = startup();//WSAStartup(ver, &data);
	if (errorCheck != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << errorCheck << std::endl;
		return 0;
	}
	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}
	//Connected to the server
	std::cout << "Connected to Server: " << ip << ":" << port << std::endl<<std::endl;
	// Do-while loop to send and receive data
	char buf[4096];
	std::string userInput;
	do
	{
		// Prompt the user for some text
		std::cout << "Send Message: ";
		
		std::getline(std::cin, userInput);
		

		if (userInput.size() > 0)		// Make sure the user has typed in something
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					std::cout << "Client: " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
	return 0;
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
