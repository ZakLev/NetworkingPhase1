// NetworkingPhase1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <WS2tcpip.h>
#include "../definitions.h"
#include "../platform.h"
#include <vector>

#pragma comment (lib, "ws2_32.lib")
//std::ifstream inFile;
//std::ofstream outFile;
bool readFile(std::string& logLines);
void writeFile(std::string line);
void clearFile();
int main()
{
	clearFile();
	std::vector<std::string> usernames;
	usernames.push_back("Server");
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
				writeFile(welcomeMsg);
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
							std::string sendMSG;
							std::ostringstream ss;
							for (int i = 0; i < ServerMaster.fd_count; i++)
							{
								SOCKET outSock = ServerMaster.fd_array[i];

								/*if (outSock != lis)
								{*/
								if (outSock == sock)
								{

								ss << "SOCKET #" << outSock << ": " << usernames[i] << " has been removed!" << std::endl;
								sendMSG = ss.str();
								send(sock, sendMSG.c_str(), sendMSG.size() + 1, 0);
								usernames.erase(std::next(usernames.begin(),i),std::next( usernames.begin(), i));
								writeFile(sendMSG);
								FD_CLR(sock, &ServerMaster);
								}

								//}
							}
							//serverOn = false;
							//break;
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

								ss << i <<". SOCKET #" << outSock << ": " << usernames[i] << std::endl;
								//}
							}
								sendMSG = ss.str();
								send(sock, sendMSG.c_str(), sendMSG.size() + 1, 0);
								writeFile(sendMSG);

						}
						else if (cmd.compare("&getlog") == 0)
						{
							std::string logLines;
							if (readFile(std::ref(logLines)))
							{
								send(sock, logLines.c_str(), logLines.size() + 1, 0);
								logLines = "Successfully transfered the Log File! :)\n";
								//send(sock, logLines.c_str(), logLines.size() + 1, 0);
								writeFile(logLines);
							}
							else
							{
								logLines = "Failed to Open/Read File! :(\n";
								send(sock, logLines.c_str(), logLines.size() + 1, 0);
								writeFile(logLines);
							}
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

									//usernames[clientAmount] = user;
									usernames.push_back(user);
									clientAmount++;
									std::string connected = "\nUsername set to " + user;
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
									send(sock, connected.c_str(), connected.size() + 1, 0);
									writeFile(connected);
								}
								else
								{
									std::string connected = "\nToo many Users Connected! ";
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
									send(sock, connected.c_str(), connected.size() + 1, 0);
									writeFile(connected);
									FD_CLR(sock, &ServerMaster);
									
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
							writeFile(sendMSG);
							}


						//}
					}
				}
			}

		}
	}

	

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
		writeFile(LeaveMSG);
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
bool readFile(std::string& logLines)
{
	bool fileRead = false;

	std::vector<std::string> logFile;
	std::ifstream inFile;
	inFile.open("ServerLog.txt");
	if (inFile.is_open())
	{
		//fileRead = true;
			std::string line;
		while (std::getline(inFile, line))
		{
			//getline(inFile, line);
		/*for (std::string line; getline(inFile, line);)
		{*/
			std::istringstream iss(line);
			logFile.push_back(line);
			if (fileRead == false && line.compare("")!=0)
			{
				fileRead = true;
			}
		//}
		}
		inFile.close();
		std::ostringstream ss;
		for (std::string l : logFile)
		{
			ss << l << std::endl;
		}
		logLines = ss.str();
	}
	return fileRead;
}
void clearFile()
{
	std::ofstream outFile;
	outFile.open("ServerLog.txt");
	if (outFile.is_open())
	{
		outFile.clear();
	}

}
void writeFile(std::string line)
{
	std::ofstream outFile;
	
	outFile.open("ServerLog.txt",std::ios::app);
	if (outFile.is_open())
	{
		/*std::string read;
		if (readFile(std::ref(read)))
		{

		std::ostringstream ss;
		ss << read << std::endl << line;
		outFile << ss.str();
		}
		else
		{*/
			outFile << line << std::endl;

		//}



		outFile.close();
	}
	else
	{
		std::cout << "Failed to Open/Write to File\n";
	}


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
