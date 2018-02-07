#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include<cstddef>



int main(int argc, char *argv[]) {

	int errno;	


	struct sockaddr_storage their_addr;
    	socklen_t addr_size;
	struct addrinfo addr, *addrPointer;
	int fileDescriptor, new_fd;
	char* portNumberPointer;
	memset(&addr, 0, sizeof addr);
	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;     // fill in my IP for me	

	portNumberPointer = argv[1];

	getaddrinfo(NULL, portNumberPointer, &addr, &addrPointer);
	
	fileDescriptor = socket(addrPointer->ai_family, addrPointer->ai_socktype, addrPointer->ai_protocol);

	bind(fileDescriptor, addrPointer->ai_addr, addrPointer->ai_addrlen); // Returns -1 on error
	

	std::cout << listen(fileDescriptor, 5);
	addr_size = sizeof(their_addr);	std::cout << "Helldsadsao" << std::endl;

	 fd_set fds;
	 struct timeval tv;
	 tv.tv_sec = 50; // timeout in seconds
	tv.tv_usec = 0; // microseconds added to timeout

	
	while(1) {
	std::cout << select(fileDescriptor, &fds, NULL, NULL, &tv);
   	//new_fd = accept(fileDescriptor, (struct sockaddr *)&their_addr, &addr_size);
   	break;
	}
	return 0;


}
