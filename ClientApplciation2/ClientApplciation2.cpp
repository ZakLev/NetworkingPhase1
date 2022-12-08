// NetworkingPhase2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <WS2tcpip.h>
#include "../definitions.h"
#include "../platform.h"
#pragma comment(lib,"WS2_32")//necessary for linkers


void writeFile(std::string line, std::string name);

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
	std::cout << "Connected to Server: " << ip << ":" << port << std::endl << std::endl;
	// Do-while loop to send and receive data
	char buf[4096];
	//char bufr[4096];
	std::string userInput;
	bool forceCall = true;
	bool logFile = false;
	bool quit = false;
	//bool registerd = false;
	int bytesReceived = recv(sock, buf, 4096, 0);
	if (bytesReceived > 0)
	{
		// Echo response to console
		std::cout << std::string(buf, 0, bytesReceived) << std::endl;
	}
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
		if (userInput.compare("&quit") == 0)
		{
			quit = true;
		}


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
					if (logFile == true)
					{
						writeFile(buf, name);
						logFile = false;
						std::cout << "File logged Successfully!" << std::endl;

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
					std::cout << "RECV failed with error" + WSAGetLastError() << std::endl;
				}
			}

		}
		else if (quit == false)
		{
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
	} while (userInput.size() > 0 && quit == false);

	// Gracefully close down everything

	closesocket(sock);
	WSACleanup();
	return 0;
}


void writeFile(std::string line, std::string name)
{
	std::ofstream outFile;
	std::string fileName = name + "Log.txt";
	outFile.open(fileName);//new File not replace
	if (outFile.is_open())
	{
		outFile << std::endl << line << std::endl;
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
