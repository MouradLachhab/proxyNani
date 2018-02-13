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

#define MAXSINGLEREAD 2000
#define MAXBUFFERSIZE 16384
class Handler {

public:
	Handler();
	void handleRequest(char* request, int requestSize, int firefoxFD);
	int checkForBadWords(char* request);
private:
	int communicationFD, bytesRead;
	char* clientBuff[MAXBUFFERSIZE];
};


#endif 