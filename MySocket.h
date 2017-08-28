#ifndef MYSOCKET_H
#define MYSOCKET_H
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#define DEFAULT_SIZE 20

enum SocketType {CLIENT, SERVER};
enum ConnectionType {TCP, UDP};

class MySocket {
	private:
		char* buffer;
		SOCKET WelcomeSocket = SOCKET_ERROR;
		SOCKET ConnectionSocket = SOCKET_ERROR;
		struct sockaddr_in SvrAddr;
		SocketType mySocket;
		std::string IpAddr;
		int port;
		ConnectionType connectionType;
		bool TCPConnect = false;
		int maxSize;
		WSADATA wsaData;
	public:
		MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
		~MySocket();
		void ConnectTCP();
		void DisconnectTCP();
		void SendData(const char*, int);
		int GetData(char*);
		std::string GetIPAddr();
		void SetIPAddr(std::string);
		void setPort(int);
		int GetPort();
		SocketType GetType();
		void SetType(SocketType);
};

#endif
