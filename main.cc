#include "proxy.h"


int main(int argc, char *argv[]) {

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

	Proxy ourServer(portNumberPointer);

	ourServer.startServer();
	std::cout << "Server Running\n\n";

	// Server will keep running until it encounters a problem with accept from the browser
	while (1) {
		if(ourServer.handleRequest() == -1) {

			std::cout << std::endl << "Now Exiting." << std::endl;
			ourServer.stop();
			return 1;
		}
	}
	
	return 0;
}