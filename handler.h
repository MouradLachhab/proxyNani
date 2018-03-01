#ifndef HANDLER_H
#define HANDLER_H

#include <cstddef>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>

#include <string.h>
#include <algorithm>
#include <sstream>

#define INITIALSIZEBUFF 5000
#define SINGLEREADSIZE 1000

int requestOver(char*);

class Handler {

public:
	Handler();
	void handleRequest(char* request, int requestSize, int firefoxFD);
	int getHost(char* request);
	int startConnection();
	void communicate(char* request);
	int checkForBadWords(std::string& request, int version);
	void refuseConnection(int version);

private:
	int connectionFD, hostFD, error;
	char* clientBuff;

	std::string hostName, portNumber;

	struct addrinfo addr, *addrPointer, *p;
	struct sockaddr_storage connectingAddress;	 // Storage for host information
	socklen_t addressSize;


};


#endif 