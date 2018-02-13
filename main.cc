#include "proxy.h"


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
	int noProblem = 1;
	Proxy ourServer(portNumberPointer);

	ourServer.startServer();

	while (1) {	
		if(ourServer.handleRequest() == -1) {

			std::cout << std::endl << "Now Exiting." << std::endl;
			ourServer.stop();
			return 1;
		}
	}
	

	return 0;
}