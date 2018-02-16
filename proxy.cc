#include "proxy.h"

int requestOver(char* buf) {

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

	memset(&addr, 0, sizeof(addr)); // Empties ?

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

    sa.sa_handler = sigchld_handler; // reap all dead processes

    // Ask the teacher about what this segment does, not too sure
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // At this point, we are ready to take connections, the program will hang until one is tried
    std::cout << "Waiting for request from Firefox." << std::endl;
    return 0;
}

// So far, the proxy is able to wait for a request, then when you do a request, it will pull it out (maybe not completel, needs to be fixed)
// and puts it out on screen. It does not send the request to the internet yet. Run it and you will see what information we get.

int Proxy::handleRequest() {

	addressSize = sizeof(connectingAddress);
   	firefoxFD = accept(serverFD, (struct sockaddr *)&connectingAddress, &addressSize);
   	if (firefoxFD == -1) {
            perror("accept");
            return -1;
        }

   //	std::cout << "FileDescriptor: " << firefoxFD; 
   //	std::cout << " - Incoming Request" << std::endl;

   	memset(&serverBuff, 0, sizeof(serverBuff)); // Clears the buffer
   	
   	bytesRead = 0;
   	do {

	   	bytesRead += recv(firefoxFD, serverBuff + bytesRead, MAXSINGLEREAD-1,0);

	   	if(bytesRead < 0 && errno != EAGAIN){ //EAGAIN just means there was nothing to read
	            perror("recv from browser failed");
	            return 1;
	    }
    } while(!requestOver(serverBuff));


    if (!fork()) { // Forking process from Beej's guide
    		//std::cout << serverBuff;
            close(serverFD); // child doesn't need the listener
        	handler.handleRequest(serverBuff, bytesRead, firefoxFD);
            close(firefoxFD);		// Close the file Descriptor once done
            exit(0);	// Exit the child process
        }
    close(firefoxFD);  // parent doesn't need the child's file descriptor
    return 0;
}

void Proxy::stop() {

	close(serverFD);
}



	






/*





	//while(1) { While removed while we test a single request
	


   	// This line is useless, just allows us to see the IP of who is making a request
   	//std::cout  << inet_ntop(connectingAddress.ss_family, get_in_addr((struct sockaddr *)&connectingAddress), s, sizeof s); // Convert request to TEXT

   	//break;
	//}


   	/* CLIENT SIDE */

   	    // initialize data buffer
    //

   // fcntl(communicationFD, F_SETFL, O_NONBLOCK);  Don't know what this does completely so it's in comment while I test
    
    /*








	close(serverFD);


	return 0;


}*/
