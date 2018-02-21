#ifndef PROXY_H
#define PROXY_H

#include "handler.h"



class Proxy {

public:
	Proxy(char* portNumber);


	int startServer();
	int handleRequest();
	void stop();

private:

	Handler handler;


	int error, yes, bytesRead;	// Yes is used to allow us to reuse the port	
	int serverFD, firefoxFD;     // File descriptor used
	char serverBuff[MAXBUFFERSIZE];     // Buffers for data content

	//Server specifics
	char* portNumberPointer;

	struct sockaddr_storage connectingAddress;	 // Storage for connection information
    socklen_t addressSize;
	struct addrinfo addr, *addrPointer, *p;		 // Some useful addrinfo
	struct sigaction sa;						 // Used for signals

	 char s[INET6_ADDRSTRLEN]; // ???


};


#endif 