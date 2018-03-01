#include "handler.h"

int requestOver2(char* buf) {

	// if we find the end characters for a HTTP header, we're done
	if (strstr(buf, "\r\n\r\n") == NULL)
		return 0;
	else
		return 1;
}


// get sockaddr, IPv4 or IPv6:
void * get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


Handler::Handler() {
	ufds.events = POLLIN; // Check for possible receive
}


void Handler::handleRequest(char* request, int requestSize, int fd) {

	hostFD = fd;

	// Try to get the host from the request
	if (getHost(request))
		return;

	std::cout << request;

	std::cin >> fd;

	// Try to connect to the host
	if(startConnection())
		return;

	// Send the request and wait for an answer
	communicate(request);

	// Once we're done, we make sure to free the dynamically allocated memory
	free(clientBuff);
	hostName = "";

	return;
}

int Handler::getHost(char* request) {

	// This is a copy so that the original does not get affected
	std::string requestString(request); 

	
	// check for a badword
	if (checkForBadWords(requestString, 0))
		return 1;

	// Get the host name from the copy
	std::istringstream iss(requestString);
    while (std::getline(iss, hostName, '\n'))
    {
        if (hostName.find("host") != std::string::npos) {
        	break;
        }
    }

    // Remove the http: header
    hostName.erase (0,5);

    // Check if there is a specific port number
    std::size_t portPosition = hostName.find(":");
    if (portPosition != std::string::npos)
    {
    	portNumber = hostName.substr(portPosition + 1, hostName.size()).c_str();
    	portNumber.resize(portNumber.size()-1); 					      // remove null character at the end
    	hostName.erase (hostName.begin() + portPosition, hostName.end()); // Seperate host and port number
    	
    }
    else 
    {
    	portNumber = "80";							// Use default server port
	    hostName.resize(hostName.size()-1); 		// remove null character at the end
	}

	// Change Connection type of old request
	std::string requestClose(request); 
		std::cout << requestClose << "\n";
	// Make connection close
	std::size_t connectionType = requestClose.find("keep");

    if (connectionType != std::string::npos)
    {
    	std::string close = "close";
    	requestClose.replace(connectionType, (size_t)10, close.c_str());
    }
    if (1) // requestClose.find("close") == std::string::npos
    {
    	std::string close = "Connection: close";
    	requestClose.insert(size_t(requestClose.length()-3), close);

	}
	strcpy(request, requestClose.c_str());

	return 0;
}

int Handler::startConnection() {

	// make sure it's empty
	memset(&addr, 0, sizeof(addr));

	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;

	// Get connection information automatically
	if ((error = getaddrinfo(hostName.c_str(), portNumber.c_str(), &addr, &addrPointer)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error)); // Error syntax from Beej's guide
        return 1;
    }
    // loop through all the results and bind to the first we can (Adapted from Beej's guide)
    for(p = addrPointer; p != NULL; p = p->ai_next) {

    	// Get a file descriptor for out socket
        if ((connectionFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {

            perror("client: socket");
            continue;
        }
        if (connect(connectionFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(connectionFD);
            perror("client: connect");
            continue;
        }
        break;
    }

    // If we could not do all three steps with any of the results, then we quit
    if (p == NULL)  {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }

    freeaddrinfo(addrPointer); // all done with this structure

    return 0;
}

void Handler::communicate(char* request) {


	// Send query
	if (send(connectionFD, request, strlen(request), 0) == -1) {
        perror("send");
		return;
	}
	std::cout<< "request sent \n";
	// Dynamically allocate memory for the answer buffer
	clientBuff = (char*)malloc(INITIALSIZEBUFF * sizeof(*clientBuff));
	int currentSize = INITIALSIZEBUFF;
	int bytesRead = 0;

	// Make sure it's empty
   	memset(clientBuff, 0, INITIALSIZEBUFF);
   	addressSize = sizeof(connectingAddress);

   	// Loop as long as we have more information coming with a timeout of 100 ms to not be too slow
   	
   	do {
	   	bytesRead += recv(connectionFD, &clientBuff[bytesRead], SINGLEREADSIZE, 0);

	   	if(bytesRead < 0 && errno != EAGAIN){ //EAGAIN just means there was nothing to read
            perror("recv from host failed");
            return;
	    }
    } while(!requestOver2(clientBuff)); // Keep reading until we have the full Header


/*
   	while) {
   		int receiveValue = recv(connectionFD, &clientBuff[bytesRead], SINGLEREADSIZE, 0);

   		printf("%d\n", receiveValue);
	    if (receiveValue == -1) {
	        std::cout << "Error";
	        perror("recv");
	        return;
	    }
	    else if ()
	    bytesRead += receiveValue;
		//std::cout << clientBuff<< "\n";
	    // if we're close to filling the buffer, we increase it's size
	    if (bytesRead >= currentSize - SINGLEREADSIZE) {
	    	currentSize += INITIALSIZEBUFF;
	    	clientBuff = (char*)realloc(clientBuff, currentSize * sizeof(*clientBuff));
	    }
	}*/

	// Make sure the last character is null
	clientBuff[bytesRead] = '\0';
	std::cout << clientBuff<< "\n";
	// Make a copy of the answer to check for bad words
	std::string answerString(clientBuff);

	// Only filter words if the content is text type
	if (answerString.find("text") != std::string::npos) {
		if (checkForBadWords(answerString, 1)) {
			return;
		}
	}
	// Send answer to the Browser
	if (send(hostFD, clientBuff, bytesRead, 0) == -1)
        perror("send");

	return;
}

int Handler::checkForBadWords(std::string& stringToCheck, int version) {

	std::string badwords[]  = {"spongebob", "britneyspears", "parishilton", "norrköpping", "norrk\045c3\045b6ping"};

	std::transform(stringToCheck.begin(), stringToCheck.end(), stringToCheck.begin(), ::tolower); // Make everything lowercase
	stringToCheck.erase(std::remove(stringToCheck.begin(), stringToCheck.end(), ' '), stringToCheck.end()); // Remove spaces
	stringToCheck.erase(std::remove(stringToCheck.begin(), stringToCheck.end(), '+'), stringToCheck.end()); // Remove + sign used by google when using spaces

	// Check if we can find any bad words
	for (int i = 0; i < 5; ++i) {
		if (stringToCheck.find(badwords[i]) != std::string::npos) {
			refuseConnection(version);
			return 1;
		}
	}
	return 0;
}

// Used to redirect the Browser if there are bad words based on URL or content
void Handler::refuseConnection(int version) {

	std::cout << "Bad word detected: Redirecting \n\n";
	std::string errorNumber;
	if(version)
	 	errorNumber = "error2.html\r\n\r\n";
	else
		errorNumber = "error1.html\r\n\r\n";

	std::string redirect = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/" + errorNumber;

	if (send(hostFD, redirect.c_str(), redirect.length(), 0) == -1)
        perror("send");

    return;
}