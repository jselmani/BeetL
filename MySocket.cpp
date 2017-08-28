#include "MySocket.h"

/****************************
  CONSTRUCTOR/DESTRUCTOR
****************************/

MySocket::MySocket(SocketType sType, std::string ip, unsigned int portNum, ConnectionType cType, unsigned int max)
{
	//start up the DLL's
	if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0)
	{
		std::cout << "DLL Start-Up Failed!" << std::endl;
		std::cin.get();
		exit(0); //exit codes represent where failure occurred, incrementing by 1
	}

	if (sType == SERVER || sType == CLIENT)
	{
		this->SetType(sType);
		this->SetIPAddr(ip);
		this->setPort(portNum);
		this->SvrAddr.sin_family = AF_INET;
		this->SvrAddr.sin_addr.s_addr = inet_addr(this->IpAddr.c_str());
		this->SvrAddr.sin_port = htons(this->port);

		if (max > 0) //max must be greater than 0 to allocate size of buffer
		{
			this->maxSize = max;
			this->buffer = new char[this->maxSize];
		}
		else
		{
			this->maxSize = DEFAULT_SIZE;
			this->buffer = new char[DEFAULT_SIZE]; //incorrect value passed in, allocate default size required
		}

		if (cType == TCP || cType == UDP)
			this->connectionType = cType;

		if (this->mySocket == SERVER)
		{
			if (this->connectionType == TCP)
			{
				this->WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

				if (this->WelcomeSocket == INVALID_SOCKET)
				{
					WSACleanup();
					exit(1);
				}

				if (bind(this->WelcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
				{
					closesocket(this->WelcomeSocket);
					WSACleanup();
					exit(2);
				}

				if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR)
				{
					closesocket(this->WelcomeSocket);
					WSACleanup();
					exit(3);
				}
				else
					std::cout << "Waiting for Client Connection..." << std::endl;

				this->ConnectTCP(); //accept the connection in this function
									//initialize the ConnectionSocket
			}
			else if (this->connectionType == UDP) //skip TCP connection
			{
				this->ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

				if (this->ConnectionSocket == INVALID_SOCKET)
				{
					WSACleanup();
					exit(4);
				}

				if (bind(this->ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
				{
					closesocket(this->ConnectionSocket);
					WSACleanup();
					exit(5);
				}
			}
		}
		else if (this->mySocket == CLIENT)
		{
			if (this->connectionType == TCP)
			{
				this->ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

				if (this->ConnectionSocket == INVALID_SOCKET)
				{
					WSACleanup();
					exit(6);
				}

				//this->ConnectTCP();
			}
			else if (this->connectionType == UDP)
			{
				this->ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

				if (this->ConnectionSocket == INVALID_SOCKET)
				{
					WSACleanup();
					exit(7);
				}
			}

		}
	}
	else
	{
		std::cout << "Invalid Socket Type.  Please press enter and start again." << std::endl;
		std::cin.get();
		exit(5);
	}
}

MySocket::~MySocket()
{
	delete [] buffer;
	buffer = nullptr;
}

/****************************
	CONNECTIONS
****************************/
void MySocket::ConnectTCP()
{
	if (this->mySocket == SERVER
		&& this->connectionType == TCP
		&& this->TCPConnect == false)
	{
		//accept an incoming connection from a client
		this->ConnectionSocket = SOCKET_ERROR;
		if ((ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			exit(8);
		}
		else
		{
			this->TCPConnect = true;
			std::cout << "Connection Accepted." << std::endl;
		}
	}
	else if (this->mySocket == CLIENT
		&& this->connectionType == TCP
		&& this->TCPConnect == false)
	{
		if ((connect(this->ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
		{
			closesocket(this->ConnectionSocket);
			WSACleanup();
			exit(9);
		}
		else
		{
			this->TCPConnect = true;
			std::cout << "Connection Established." << std::endl;
		}
	}
}

void MySocket::DisconnectTCP()
{
	if (this->TCPConnect == true)
	{
		if (this->mySocket == SERVER)
		{
			closesocket(this->ConnectionSocket); //close inbound socket
			closesocket(this->WelcomeSocket); //close server socket
			WSACleanup(); //free WinSock resources
			this->TCPConnect = false;
		}
		else if (this->mySocket == CLIENT)
		{
			closesocket(this->ConnectionSocket);
			WSACleanup();
			this->TCPConnect = false;
		}
	}
}

/*****************************
	SEND/RECEIVE
*****************************/
void MySocket::SendData(const char* buff, int size)
{
	this->buffer = new char[size];
	memcpy(this->buffer, buff, size);

	if (this->connectionType == TCP)
		send(ConnectionSocket, this->buffer, size, 0);
	else if (this->connectionType == UDP)
		sendto(ConnectionSocket, this->buffer, size, 0, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
}

int MySocket::GetData(char* buff)
{
	memset(buff, '\0', maxSize);
	this->buffer = new char[this->maxSize];
	char* ptr = this->buffer;
	int size = 0;

	if (this->connectionType == TCP)
	{
		recv(this->ConnectionSocket, this->buffer, DEFAULT_SIZE, 0);
	}
	else if (this->connectionType == UDP)
	{
		int addr_len = sizeof(SvrAddr);
		recvfrom(this->ConnectionSocket, this->buffer, DEFAULT_SIZE, 0, (struct sockaddr*)&SvrAddr, &addr_len);
	}

	memcpy(buff, this->buffer, DEFAULT_SIZE);

	return size;
}

/********************************
	SET/GET
*********************************/

void MySocket::SetIPAddr(std::string ip)
{
	if (this->TCPConnect == false || this->WelcomeSocket == SOCKET_ERROR)
	{
		this->IpAddr = ip.c_str();
	}
	else
		std::cout << "A connection is already established. You cannot change the IP Address now." << std::endl;
}

//returns the IP Address configured within the MySocket object
std::string MySocket::GetIPAddr()
{
	return this->IpAddr;
}

void MySocket::setPort(int portNum)
{
	if (this->TCPConnect == false || this->WelcomeSocket == SOCKET_ERROR)
	{
		this->port = portNum;
	}
	else
		std::cout << "A connection is already established. You cannot change the Port Number now." << std::endl;
}

int MySocket::GetPort()
{
	return this->port;
}

void MySocket::SetType(SocketType sType)
{
	if (this->TCPConnect == false
		|| this->WelcomeSocket == SOCKET_ERROR
		|| this->ConnectionSocket == SOCKET_ERROR)
	{
		this->mySocket = sType;
	}
	else
		std::cout << "Connection established.  Cannot change the sockets type" << std::endl;
}

SocketType MySocket::GetType()
{
	return this->mySocket;
}
