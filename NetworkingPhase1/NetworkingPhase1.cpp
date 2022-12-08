// NetworkingPhase1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <WS2tcpip.h>
#include "../definitions.h"
#include "../platform.h"

#pragma comment (lib, "ws2_32.lib")

int main()
{
	std::string usernames[6];
	usernames[0] = "Server";
	int clientAmount = 1;
	 std::string ip = "127.0.0.1";			// Server IP Address
	int port = 3333;
	std::string sOut;
	sOut = usernames[0] + ": Waiting For Connection\n" + ip + "\n" + std::to_string(port) + "\n";
	std::cout << sOut;

	int errorCheck = startup();
    if (errorCheck != 0)
    {

		sOut + usernames[0] +": Can't start Winsock, Exit\n";
		std::cout << sOut;
        return 0;
    }
	// Create socket
	SOCKET lis = socket(AF_INET, SOCK_STREAM, 0);
	if (lis == INVALID_SOCKET)
	{
		sOut = usernames[0] + "Can't create socket, Exit\n";
		std::cout << sOut;
		return 0;
	}

	//Bind
	sockaddr_in hin;
	hin.sin_family = AF_INET;
	hin.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &hin.sin_addr);

	bind(lis, (sockaddr*)&hin, sizeof(hin));

	// listening
	listen(lis, SOMAXCONN); // amount to listen
	
	fd_set ServerMaster;
	FD_ZERO(&ServerMaster);

	FD_SET(lis, &ServerMaster);

	bool serverOn = true;
	while (serverOn)
	{
			fd_set copyServerMaster = ServerMaster;
		timeval time = {1,0};
		int sockCount = select(0, &copyServerMaster, nullptr, nullptr, &time);

		for (int i = 0; i < sockCount; i++)
		{
			SOCKET sock = copyServerMaster.fd_array[i];
			if (sock == lis)
			{

				SOCKET sockClient = accept(lis, nullptr, nullptr);

				FD_SET(sockClient, &ServerMaster);
				
				std::string welcomeMsg = "Welcome The Client has Connected to the Server!\n";

				send(sockClient, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else
			{
				char buff[4096];
				ZeroMemory(buff, 4096);

				// Receive message
				int bytesIn = recv(sock, buff, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &ServerMaster);
				}
				else
				{
					// Check to see if it's a command. &quit kills the server
					if (buff[0] == '&')
					{
						// Is the command quit? 
						std::string cmd = std::string(buff, bytesIn);
						cmd.erase(cmd.size()-1,cmd.size());
						//cmd = "&register";
						if (cmd.compare("&quit")==0)
						{
							serverOn = false;
							break;
						}
						else if (cmd.compare("&getlist") == 0)
						{
							
							std::string sendMSG;
								std::ostringstream ss;
							for (int i = 0; i < ServerMaster.fd_count; i++)
							{
								SOCKET outSock = ServerMaster.fd_array[i];

								/*if (outSock != lis)
								{*/

								ss << "SOCKET #" << outSock << ": " << usernames[i] << std::endl;
								//}
							}
								sendMSG = ss.str();
								send(sock, sendMSG.c_str(), sendMSG.size() + 1, 0);

						}
						else if (cmd.compare("&getlog") == 0)
						{

						}
						else if (cmd.compare("&register") == 0)
						{
							std::string connected = "\nPlease Enter A Username: ";
							//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
							send(sock, connected.c_str(), connected.size() + 1, 0);
							ZeroMemory(buff, 4096);
							//char buff[4096];
							int bytesIn = recv(sock, buff, 4096, 0);
							if (bytesIn > 0)
							{
								std::string user = std::string(buff, bytesIn);
								user.erase(user.size() - 1, user.size());
								if (clientAmount < 5)
								{

									usernames[clientAmount] = user;
									clientAmount++;
									std::string connected = "\nUsername set to " + user;
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
									send(sock, connected.c_str(), connected.size() + 1, 0);
								}
								else
								{
									std::string connected = "\nToo many Users Connected! ";
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
									send(sock, connected.c_str(), connected.size() + 1, 0);
								}


							}
						}
						

					

						// Unknown command
						continue;
						//goto start;
					}

					// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < ServerMaster.fd_count; i++)
					{
						SOCKET outSock = ServerMaster.fd_array[i];
						std::string sendMSG;
						//if (outSock != sock )//&& outSock != lis)
						//{
							std::ostringstream ss;

							if (outSock == sock)
							{

							ss << "ECHO: SOCKET #" << sock << " "<<usernames[i]<< ": " << buff << "\r\n";
							 sendMSG = ss.str();
							send(outSock, sendMSG.c_str(), sendMSG.size() + 1, 0);
							}


						//}
					}
				}
			}

		}
	}

	//}
	////wait
	//sockaddr_in client;
	//int clientSize = sizeof(client);
	//
	//SOCKET clientSock = accept(lis, (sockaddr*)&client, &clientSize);
	//
	//char host[NI_MAXHOST];
	//char service[NI_MAXSERV];

	//ZeroMemory(host, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	std::cout << host << "Connected on Port: " << service << std::endl;
	//}
	//else
	//{
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	std::cout << host << "Connected on Port: " << ntohs(client.sin_port) << std::endl;
	//}
	//closesocket(lis);

	//char buff[4096];// = { 0, };
	//while (true)
	//{
	//	ZeroMemory(buff, 4096);
	//	int bytes = recv(clientSock, buff, 4096, 0);
	//	if (bytes == SOCKET_ERROR)
	//	{
	//		std::cerr << "\nError in recV, Exit" << std::endl;
	//		break;
	//	}
	//	if (bytes == 0)
	//	{
	//		std::cout << "\nDisconnected" << std::endl;
	//		break;
	//	}
	//	std::cout << "Client(" << host << "): " << std::string(buff, 0, bytes) << std::endl;

	//	send(clientSock, buff, bytes + 1, 0);
	//}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(lis, &ServerMaster);
	closesocket(lis);

	// Message to let users know what's happening.
	std::string LeaveMSG = "Server is disconnected.\n";

	while (ServerMaster.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = ServerMaster.fd_array[0];

		// Send the goodbye message
		send(sock, LeaveMSG.c_str(), LeaveMSG.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &ServerMaster);
		closesocket(sock);
	}

	// Gracefully close down everything
    //closesocket(clientSock);
	WSACleanup();
	system("pause");
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
