// NetworkingPhase2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <WS2tcpip.h>
#include <thread>
#include <chrono>
#include "../definitions.h"
#include "../platform.h"
#pragma comment(lib,"WS2_32")//necessary for linkers


void writeFile(std::string line, std::string name);
void DisplayErrorInfo();
int main()
{
	////Get IP and PORT
	
	bool registered = false;
	std::string name;
	std::string ip, holdPort;
	int port;
	std::cout << "Enter the ip: \n";
	std::getline(std::cin, ip);
	std::cout << std::endl;
	std::cout << "Enter the Port: \n";
	std::getline(std::cin, holdPort);
	port = stoi(holdPort);
	std::cout << std::endl;



	//ip = "127.0.0.1";			// IP of server for testing
	//port = 3333; // port of server for testing
	/*WSAData data;
	WORD ver = MAKEWORD(2, 2);*/
	int errorCheck = startup();//WSAStartup(ver, &data);
	if (errorCheck != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << errorCheck << std::endl;
		DisplayErrorInfo();
		return 0;
	}
	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		//std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		DisplayErrorInfo();
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
		//std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		DisplayErrorInfo();
		closesocket(sock);
		WSACleanup();
		return 0;
	}
	//Connected to the server
	std::cout << "Connected to Server: " << ip << ":" << port << std::endl << std::endl;
	// Do-while loop to send and receive data
	//char buf[4096];
	//char bufr[4096];
	std::string userInput;
	bool forceCall = true;
	bool logFile = false;
	bool quit = false;
	bool list = false;
	//bool registerd = false;
	//char buff[1];
	uint32_t buff = 0;
	int maxSize = 1;
	int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
	char* bufr = new char[buff+1];
	int bytesReceived = recv(sock, bufr, buff+1, 0);
	if (bytesReceived > 0)
	{
		// Echo response to console
		std::cout << std::string(bufr, 0, bytesReceived) << std::endl;
	}
	delete []bufr;
	buff = 0;
	do
	{
		/*if (registerd == true)
		{*/

		startInput:
		// Prompt the user for some text
		std::string check = userInput;
		if (registered == false && forceCall == true)
		{
			userInput = "&register";
			forceCall = false;
		}
		
		else {


	//std::atexit(exiting);
	startMSG:	std::cout << "Send Message: ";

		std::getline(std::cin, userInput);
		//	check.erase(check.size() - 1, check.size());
			if (userInput.size() >  (sizeof(uint32_t) * 7))
			{
				std::cout << "Message too Large!\n";
				userInput = check;
				goto startMSG;
			}
		if (registered == false)
		{
			name = userInput;
			registered = true;
		}
		if (userInput.compare("&register") == 0)
		{
			if (registered == true)
			{

				std::cout << "\nAlready registerd!\n";
				//userInput.clear();
				//userInput.resize(0);
				userInput = check;
				goto startMSG;
				//break;
			}
			else
			{
				std::cout << "\nAlready registerd!\n";
				registered = true;
				userInput = check;
				goto startMSG;
				
			}
		}

		}
		if (userInput.compare("&getlog") == 0)
		{
			logFile = true;
		}
		else if (userInput.compare("&quit") == 0)
		{
			quit = true;
		}
		else if (userInput.compare("&getlist") == 0)
		{
			list = true;
		}
		

		if (userInput.size() > 0)		// Make sure the user has typed in something
		{
			//Send Text Size
			std::string ResultSize = std::to_string(userInput.size());
			int sendResultSize = send(sock, ResultSize.c_str(), maxSize, 0);

			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				//ZeroMemory(buf, 4096);
				//char buff[1];
				//ZeroMemory(buff, 1);
				buff = 0;
				int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
				char* buf = new char[buff+1];
				int bytesReceived = recv(sock, buf, buff+1, 0);
				//int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console
					if (logFile == true)
					{
						std::cout << std::string(buf, 0, bytesReceived) << std::endl;
						bool log = true;
						//delete[]buf;
						while (log)
						{
							buff = 0;
							int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
							char* buf2 = new char[buff+1];
							int bytesReceived = recv(sock, buf2, buff+1, 0);
							std::string line = std::string(buf2, 0, bytesReceived);
							
							if (line.compare("EOF") == 0)
							{
								log = false;
							}
							else
							{
								//std::cout << line << std::endl;
								writeFile(line, name);
							}
							delete[]buf2;
						}
						logFile = false;
						//std::cout << "File logged Successfully!" << std::endl;
						//delete[]buf;
						buff = 0;
						int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
						char* buf2 = new char[buff+1];
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
						int bytesReceived = recv(sock, buf2, buff+1, 0);
						std::cout << std::string(buf2, 0, bytesReceived) << std::endl;
						delete []buf2;
						buff = 0;

					}
					else if (list == true)
					{
						std::cout << std::string(buf, 0, bytesReceived) << std::endl;
						bool gettingList = true;
						while (gettingList)
						{
							buff = 0;
							int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
							char* buf2 = new char[buff + 1];
							int bytesReceived = recv(sock, buf2, buff + 1, 0);
							std::string line = std::string(buf2, 0, bytesReceived);

							if (line.compare("EOL") == 0)
							{
								gettingList = false;
							}
							else
							{
								std::cout << line;
							}
							delete[]buf2;
						}
						list = false;
						/*buff = 0;
						int bytesRec = recv(sock, (char*)&buff, maxSize, 0);
						char* buf2 = new char[buff + 1];
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
						int bytesReceived = recv(sock, buf2, buff + 1, 0);
						std::cout << std::string(buf2, 0, bytesReceived) << std::endl;
						delete[]buf2;
						buff = 0;*/
					}
					else
					{
						
						std::cout << std::string(buf, 0, bytesReceived) << std::endl;
					}
				}
				else if (bytesReceived == 0)
				{
					std::cout << "Connect Broke!\n";
					//userInput = check;
					//goto startMSG;
				}
				else
				{
					//std::cout << "RECV failed with error" + WSAGetLastError() << std::endl;
					DisplayErrorInfo();
				}
			delete[]buf;
			buff = 0;
			}
		}
		else if(quit == false)
		{
			buff = 0;
			goto startInput;
		}

		

		/*else
		{
			userInput = "&register";
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);
			registerd = true;
		}*/
		buff = 0;
	} while (userInput.size() > 0 && quit == false);
	
	// Gracefully close down everything
	buff = 0;
	closesocket(sock);
	WSACleanup();
	
	//ZeroMemory((char*)&buff, 1);
	//memset(&buff, 0, sizeof(buff));
	//free(&buff);
	//delete &buff;
	return 0;
}



void writeFile(std::string line, std::string name)
{
	std::ofstream outFile;
	std::string fileName = name + "Log.txt";
	outFile.open(fileName,std::ios::app);//new File not replace
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
