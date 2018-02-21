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

#include <string.h>
#include <algorithm>
#include <sstream>

#define MAXSINGLEREAD 2048
#define MAXBUFFERSIZE 16384
class Handler {

public:
	Handler();
	void handleRequest(char* request, int requestSize, int firefoxFD);
	int getHost(char* request);
	int checkForBadWords(std::string& request);
	int startConnection();
	int communicate(char* request);
private:
	struct addrinfo addr, *addrPointer, *p;
	struct sockaddr_storage connectingAddress;	 // Storage for host information
	socklen_t addressSize;
	char s[INET6_ADDRSTRLEN];
	std::string hostName;
	int connectionFD, hostFD, bytesRead, error;
	char* clientBuff[2048];

};


#endif 