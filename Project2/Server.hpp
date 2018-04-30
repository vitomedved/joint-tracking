#pragma once


//Kod preuzet sa Stack Overflow-a: https://stackoverflow.com/questions/20805732/winsock-error-10022-on-listen
//Prvi odgovor na pitanje od (Remy Lebeau)

#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <thread>
#include <string>

#define maxConnections 1

class Server
{
private:
	bool winsockStarted;
	SOCKET sock;
	
public:
	Server();
	~Server();
	bool Start(const char *port);
	void Stop();
	
};

