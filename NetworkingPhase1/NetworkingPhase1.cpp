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
#include <thread>
#include <chrono>

#pragma comment (lib, "ws2_32.lib")
//std::ifstream inFile;
//std::ofstream outFile;
bool readFile(std::string& logLines);
bool readSendFile(SOCKET sock);
void writeFile(std::string line);
void clearFile();
void DisplayErrorInfo();
int main()
{
	clearFile();
	std::vector<std::string> usernames;
	std::vector<SOCKET> userSockets;
	usernames.push_back("Server");
	
	int clientAmount = 1;
	 std::string ip = "127.0.0.1";			// Server IP Address
	int port = 3333;
	int maxSize = 1;
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
	userSockets.push_back(lis);
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

	if (bind(lis, (sockaddr*)&hin, sizeof(hin)) == SOCKET_ERROR)
		DisplayErrorInfo();

	// listening
	if (listen(lis, 4/*SOMAXCONN*/) == SOCKET_ERROR) // amount to pssible to connect
		DisplayErrorInfo();
	
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
			if (sock == SOCKET_ERROR)
				DisplayErrorInfo();
			if (sock == lis)
			{

				SOCKET sockClient = accept(lis, nullptr, nullptr);
				if(sockClient == SOCKET_ERROR)
					DisplayErrorInfo();
				FD_SET(sockClient, &ServerMaster);
				
				std::string welcomeMsg = "Welcome The Client has Connected to the Server!\n";


				std::string wMsgSize = std::to_string(welcomeMsg.size());
				send(sockClient, wMsgSize.c_str(), maxSize, 0);

				send(sockClient, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				writeFile(welcomeMsg);
			}
			else
			{
				//char buff[4096];
				//ZeroMemory(buff, 4096);
				//Receive Message Size
				//char buf[1];
				//ZeroMemory(buf, 1);
				uint32_t buf = 0;
				int bytesInSize = recv(sock, (char*)&buf, maxSize, 0);
				char* buff = new char[buf+1];

				// Receive message
				int bytesIn = recv(sock, buff, buf+1, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					for (int i = 0; i < userSockets.size(); i++)//Remove Disconnected From List
					{
						if (sock == userSockets[i])
						{
							usernames.erase(usernames.begin() + i);
							userSockets.erase(userSockets.begin() + i);
						}
					}
					closesocket(sock);//Drop Disconnected Sockets
					FD_CLR(sock, &ServerMaster);
				}
				else
				{
					// Check to see if it's a command. &quit kills the server
					if (buff[0] == '$')
					{
						// Is the command quit? 
						std::string cmd = std::string(buff, bytesIn);
						cmd.erase(cmd.size()-1,cmd.size());
						//cmd = "&register";
						if (cmd.compare("$quit")==0)
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

								std::string MsgSize = std::to_string(sendMSG.size());
								send(sock, MsgSize.c_str(), maxSize, 0);
								//std::this_thread::sleep_for(std::chrono::milliseconds(50));
								send(sock, sendMSG.c_str(), sendMSG.size() + 1, 0);
								//std::this_thread::sleep_for(std::chrono::milliseconds(50));
								usernames.erase(usernames.begin() + i);
								userSockets.erase(userSockets.begin() + i);
							
								//usernames.erase(std::next(usernames.begin(),i),std::next( usernames.begin(), i));
								//usernames.erase(std::remove(usernames.begin(), usernames.end(), i), std::remove(usernames.begin(), usernames.end(), i));
							
								/*for (std::string names : usernames)
									std::cout << names << std::endl;*/
								writeFile(sendMSG);
								FD_CLR(sock, &ServerMaster);
								}

								//}
							}
							//serverOn = false;
							//break;
						}
						else if (cmd.compare("$getlist") == 0)
						{
							//Send Loading MSG
							std::string Msg = "Sending List of Users...\n";
							std::string MsgSize = std::to_string(Msg.size());
							send(sock, MsgSize.c_str(), maxSize, 0);

							send(sock, Msg.c_str(), Msg.size() + 1, 0);
							for (int i = 0; i < ServerMaster.fd_count; i++)
							{
								SOCKET outSock = ServerMaster.fd_array[i];
								if (outSock == SOCKET_ERROR)
									DisplayErrorInfo();
								if (outSock != userSockets[i])
								{
									usernames.erase(usernames.begin() + i);
									userSockets.erase(userSockets.begin() + i);
									i--;
								}
								else
								{
							    std::string sendMSG;
								std::ostringstream ss;
								ss << i <<". SOCKET #" << outSock << ": " << usernames[i] << std::endl;
								//}
								sendMSG = ss.str();

								 MsgSize = std::to_string(sendMSG.size());
								send(sock, MsgSize.c_str(), maxSize, 0);
								 std::this_thread::sleep_for(std::chrono::milliseconds(50));
								send(sock, sendMSG.c_str(), sendMSG.size() + 1, 0);
								// std::this_thread::sleep_for(std::chrono::milliseconds(50));
								writeFile(sendMSG);
								}
								
							}
							//Send EOL MSG
							 Msg = "EOL";
							 MsgSize = std::to_string(Msg.size());
							send(sock, MsgSize.c_str(), maxSize, 0);
							send(sock, Msg.c_str(), Msg.size() + 1, 0);
							

						}
						else if (cmd.compare("$getlog") == 0)
						{
							std::string logLines;
							//if (readFile(std::ref(logLines)))
							//{
								//Send Loading MSG
								std::string Msg = "Logging...\n";
								std::string MsgSize = std::to_string(Msg.size());
								send(sock, MsgSize.c_str(), maxSize, 0);

								send(sock, Msg.c_str(), Msg.size() + 1, 0);
								
								if (readSendFile(sock))//Send MSG Line by Line
								{


								//Send Completed MSG
								std::string MSG = "Successfully transfered the Log File! :)\n";

								 MsgSize = std::to_string(MSG.size());
								send(sock, MsgSize.c_str(), maxSize, 0);
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								send(sock, MSG.c_str(), MSG.size() + 1, 0);
								//send(sock, logLines.c_str(), logLines.size() + 1, 0);
								writeFile(MSG);
								}
								else
								{
									logLines = "Failed to transfered the Log File! :(\n";

									MsgSize = std::to_string(logLines.size());
									send(sock, MsgSize.c_str(), maxSize, 0);

									send(sock, logLines.c_str(), logLines.size() + 1, 0);
									writeFile(logLines);
								}
						
						}
						else if (cmd.compare("$register") == 0)
						{
							std::string connected = "\nPlease Enter A Username: ";
							//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;

							std::string MsgSize = std::to_string(connected.size());
							send(sock, MsgSize.c_str(), maxSize, 0);

							send(sock, connected.c_str(), connected.size() + 1, 0);
							
							buf = 0;
							int bytesInSize = recv(sock, (char*)&buf, maxSize, 0);
							char* buff2 = new char[buf+1];

							int bytesIn = recv(sock, buff2, buf+1, 0);
							if (bytesIn > 0)
							{
								//for (int i = 0; i < usernames.size(); i++)//Get Rid of Disconnected Users that are at the back
								//{
								//	if (ServerMaster.fd_array[i] == NULL || ServerMaster.fd_array[i] != userSockets[i])
								//	{
								//		usernames.erase(usernames.begin() + i);
								//		userSockets.erase(userSockets.begin() + i);
								//		if (ServerMaster.fd_array[i] != NULL)
								//		{
								//			i--;
								//		}
								//	}
								//}
								std::string user = std::string(buff2, bytesIn);
								user.erase(user.size() - 1, user.size());
								if (clientAmount < 5)// Max Clients Allowed 3 - the fourth will be kicked 
								{

									//usernames[clientAmount] = user;
									usernames.push_back(user);
									userSockets.push_back(sock);
									clientAmount++;
									 connected = "\nUsername set to " + user;
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;
									std::string MsgSize = std::to_string(connected.size());
									send(sock, MsgSize.c_str(), maxSize, 0);

									send(sock, connected.c_str(), connected.size() + 1, 0);
									writeFile(connected);
								}
								else
								{
									connected = "\nToo many Users Connected!\n";
									//std::cout << sockClient << "Connected on Port: " << ntohs(sockClient.sin_port) << std::endl;

									std::string MsgSize = std::to_string(connected.size());
									send(sock, MsgSize.c_str(), maxSize, 0);


									send(sock, connected.c_str(), connected.size() + 1, 0);
									writeFile(connected);
									FD_CLR(sock, &ServerMaster);
									
								}


							}
							delete[]buff2;
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

							 std::string MsgSize = std::to_string(sendMSG.size());
							 send(outSock, MsgSize.c_str(), maxSize, 0);

							send(outSock, sendMSG.c_str(), sendMSG.size() + 1, 0);
							writeFile(sendMSG);
							}
							if (outSock == SOCKET_ERROR)
								DisplayErrorInfo();

						//}
					}
				}
			delete[]buff;
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

		std::string MsgSize = std::to_string(LeaveMSG.size());
		send(sock, MsgSize.c_str(), maxSize, 0);

		send(sock, LeaveMSG.c_str(), LeaveMSG.size() + 1, 0);
		writeFile(LeaveMSG);
		// Remove it from the master file list and close the socket
		FD_CLR(sock, &ServerMaster);
		closesocket(sock);
	}
	writeFile("Server Closed :(");
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
			std::string line;
		while (std::getline(inFile, line))
		{
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
bool readSendFile(SOCKET sock)
{
	bool fileRead = false;
	int maxSize = 1;
	//std::vector<std::string> logFile;
	std::ifstream inFile;
	inFile.open("ServerLog.txt");
	if (inFile.is_open())
	{
		std::string line;
		while (std::getline(inFile, line))
		{
			/*if (inFile.eof())
				break;*/
			std::istringstream iss(line);
			if (line.compare("") == 0||line.empty())
			{
				line = "\n";
			}
			//logFile.push_back(line);
			std::string MsgSize = std::to_string(line.size());
			send(sock, MsgSize.c_str(), maxSize, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			send(sock, line.c_str(), line.size() + 1, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (fileRead == false && line.compare("") != 0)
			{
				fileRead = true;
			}
			//}
		}
		inFile.close();
		
		line = "EOF";
		std::string MsgSize = std::to_string(line.size());
		send(sock, MsgSize.c_str(), maxSize, 0);
	
		send(sock, line.c_str(), line.size() + 1, 0);
		
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
			outFile << line << std::endl;

		outFile.close();
	}
	else
	{
		std::cout << "Failed to Open/Write to File\n";
	}


}
void DisplayErrorInfo()
{
	wchar_t* errorMessage = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		WSAGetLastError(), MAKELANGID(
			LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&errorMessage, 0, NULL);
	fprintf(stderr, "%S\n", errorMessage);
	LocalFree(errorMessage);

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
