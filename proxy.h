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

	bool blocked;
	int error, yes, bytesRead, requestNumber, counter;	// Yes is used to allow us to reuse the port	
	int serverFD, firefoxFD;  				    // File descriptors used
	char serverBuff[INITIALSIZEBUFF];            // Buffers for data content, it should fit in 5000
	char* portNumberPointer;

	struct sockaddr_storage connectingAddress;	 // Storage for connection information
    socklen_t addressSize;
	struct addrinfo addr, *addrPointer, *p;		 // Some useful addrinfo
	struct sigaction sa;						 // Used for signals
};


#endif 