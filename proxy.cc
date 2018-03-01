#include "proxy.h"

int requestOver(char* buf) {

	// if we find the end characters for a HTTP header, we're done
	if (strstr(buf, "\r\n\r\n") == NULL)
		return 0;
	else
		return 1;
}

Proxy::Proxy(char* portNumber) : portNumberPointer(portNumber)
{
	yes = 1;
}

/*************************************************************************************************/


// To understand what it does: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void sigchld_handler(int s) 
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

/*************************************************************************************************/


int Proxy::startServer() {

	// Make sure it's empty
	memset(&addr, 0, sizeof(addr));

	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;     // fill in my IP for me


	if ((error = getaddrinfo(NULL, "8080", &addr, &addrPointer)) != 0) // Get connection information automatically
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error)); // Error syntax from Beej's guide
        return 1;
    }
		
    // loop through all the results and bind to the first we can (Adapted from Beej's guide)
    for(p = addrPointer; p != NULL; p = p->ai_next) {

    	// Get a file descriptor for out socket
        if ((serverFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        // Set the options for the port so that it can be reused
        if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

       	// Try to bind the address and the file descriptor
        if (bind(serverFD, p->ai_addr, p->ai_addrlen) == -1) { 
        	close(serverFD);
            perror("server: bind");
            continue;
        }

        // If we passed all three steps, we can continuer
        break;
    }

     freeaddrinfo(addrPointer); // all done with this structure

    // If we could not do all three steps with any of the results, then we quit
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }


    // Open our port to communication
    if (listen(serverFD, 5) == -1) {
        perror("listen");
        exit(1);
    }


    // reap all dead processes that can be caused with the fork()
    sa.sa_handler = sigchld_handler; 

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // At this point, we are ready to take connections, the program will hang until one is tried

    return 0;
}

int Proxy::handleRequest() {

	addressSize = sizeof(connectingAddress);
   	firefoxFD = accept(serverFD, (struct sockaddr *)&connectingAddress, &addressSize);
    
   	if (firefoxFD == -1) {
            perror("accept");
            return -1;
        }

    // Make sure the buffer is empty
   	memset(&serverBuff, 0, sizeof(serverBuff)); // Clears the buffer
   	
   	bytesRead = 0;
   	do {
	   	bytesRead += recv(firefoxFD, serverBuff + bytesRead, sizeof(serverBuff),0);

	   	if(bytesRead < 0 && errno != EAGAIN){ //EAGAIN just means there was nothing to read
            perror("recv from browser failed");
            return 1;
	    }
    } while(!requestOver(serverBuff)); // Keep reading until we have the full Header

    if (!fork()) { 			        // Forking process from Beej's guide

            close(serverFD); 	    // child doesn't need the listener
        	handler.handleRequest(serverBuff, bytesRead, firefoxFD);

            close(firefoxFD);		// Close the file Descriptor once done
            exit(0);	 	    	// Exit the child process
        }

    close(firefoxFD);  				// parent doesn't need the child's file descriptor

    return 0;
}

void Proxy::stop() {

	close(serverFD);
}