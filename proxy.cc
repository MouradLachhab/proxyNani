#include<cstddef>
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

#include <arpa/inet.h>


/*************************************************************************************************/


// To understand what it does: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void sigchld_handler(int s) 
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/*************************************************************************************************/


// So far, the proxy is able to wait for a request, then when you do a request, it will pull it out (maybe not completel, needs to be fixed)
// and puts it out on screen. It does not send the request to the internet yet. Run it and you will see what information we get.



int main(int argc, char *argv[]) {

	/****************                         READ CAREFULLY                      ********************/
	/* The reason our proxy didn't work in class was due to the fact that it wasn't set up correctly */
	/* When setting up your proxy on firefox, there will be two field in which you have to enter     */
	/* information. In the first one, you need to enter the hostname, which is your computer's name  */
	/* If you aren't sure of the name, running the proxy will output it to you thanks to the lines   */
	/* below. Then you just enter the port number that you will use.                                 */

	char hostname[128];

	gethostname(hostname, sizeof hostname);
	printf("My hostname: %s\n", hostname);


	/*************************************************************************************************/

	char* portNumberPointer = argv[1];	// Get the port number specified by the user
	int port = strtol(portNumberPointer,NULL, 10);	// Convert it into an int for checking purposes

	// Check if the port number is valid
	if (port <= 1024 || port > 65535) 
	{
		std::cout << "Invalid Port Number: Must be greater than 1024 up to 65535." << std::endl;
		return 1;
	}




	/******************************* Variables *********************************/

	int error, yes = 1;	// Yes is used to allow us to reuse the port	
	int serverFD, communicationFD, clientFD;
	char serverBuff[1024], clientBuff[1024];

	//Server specifics
	struct sockaddr_storage connectingAddress;
    socklen_t addressSize;
	struct addrinfo addr, *addrPointer, *p;
	struct sigaction sa;

	char s[INET6_ADDRSTRLEN]; // ???

	/******************************* Variables *********************************/


	memset(&addr, 0, sizeof(addr)); // Empties ?
	memset(&serverBuff, 0, sizeof(serverBuff));

	memset(clientBuff, '0',sizeof(clientBuff));

	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;     // fill in my IP for me

	portNumberPointer = argv[1];


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

	//while(1) { While removed while we test a single request
	addressSize = sizeof(connectingAddress);
   	communicationFD = accept(serverFD, (struct sockaddr *)&connectingAddress, &addressSize);
   	std::cout << communicationFD; 
   	std::cout << "Incoming Request" << std::endl;


   	// This line is useless, just allows us to see the IP of who is making a request
   	//std::cout  << inet_ntop(connectingAddress.ss_family, get_in_addr((struct sockaddr *)&connectingAddress), s, sizeof s); // Convert request to TEXT

   	//break;
	//}


   	/* CLIENT SIDE */

   	    // initialize data buffer
    char buf[20000];

   // fcntl(communicationFD, F_SETFL, O_NONBLOCK);  Don't know what this does completely so it's in comment while I test
    
    std::cout << "Number of bytes read:" << recv(communicationFD, buf, 20000-1,0) << std::endl;
    std::cout << buf;








	close(serverFD);


	return 0;


}
