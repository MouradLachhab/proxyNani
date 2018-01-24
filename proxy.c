#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



int main() {

	
	struct sockaddr_storage their_addr;
    	socklen_t addr_size;
	struct addrinfo addr, *addrPointer;
	int fileDescriptor, new_fd;
	char portNumber;
	char* portNumberPointer;

	memset(&addr, 0, sizeof addr);
	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;     // fill in my IP for me	

	std::cout << "What port do you want to use ? (Above 1024) \n";
	std::cin >> portNumber;
	std::cout << portNumber;
	portNumberPointer = &portNumber;
	getaddrinfo(NULL, portNumberPointer, &addr, &addrPointer);

	fileDescriptor = socket(addrPointer->ai_family, addrPointer->ai_socktype, addrPointer->ai_protocol);

	
	int error = bind(fileDescriptor, addrPointer->ai_addr, addrPointer->ai_addrlen); // Returns -1 on error

	int error2 = listen(fileDescriptor, 5);

	addr_size = sizeof(their_addr);
   	new_fd = accept(fileDescriptor, (struct sockaddr *)&their_addr, &addr_size);



	return 0;


}
