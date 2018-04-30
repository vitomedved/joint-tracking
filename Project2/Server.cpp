#include "Server.hpp"

Server::Server()
	: sock(INVALID_SOCKET), winsockStarted(false)
{
	WSADATA WSAData = { 0 };
	int status = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (status != 0)
		std::cout << "[ERROR]: " << status << " Unable to start Winsock." << std::endl;
	else
		winsockStarted = true;
}

Server::~Server()
{
	Stop();

	if (winsockStarted)
		WSACleanup();
}

bool Server::Start(const char *port)
{
	Stop();

	struct addrinfo hints = { 0 };
	struct addrinfo *res = NULL;

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0)
	{
		std::cout << "[ERROR]: " << status << " Unable to get address info for Port " << port << "." << std::endl;
		return false;
	}

	SOCKET newsock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (newsock == INVALID_SOCKET)
	{
		std::cout << "[ERROR]: " << WSAGetLastError() << " Unable to create Socket." << std::endl;
		freeaddrinfo(res);
		return false;
	}

	if (bind(newsock, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "[ERROR]: " << WSAGetLastError() << " Unable to bind Socket." << std::endl;
		freeaddrinfo(res);
		closesocket(newsock);
		return false;
	}

	freeaddrinfo(res);

	if (listen(newsock, maxConnections) == SOCKET_ERROR)
	{
		std::cout << "[ERROR]: " << WSAGetLastError() << " Unable to Listen on Port " << port << "." << std::endl;
		closesocket(newsock);
		return false;
	}

	sock = newsock;
	return true;
}

void Server::Stop()
{
	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
}