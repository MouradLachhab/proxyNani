#include "handler.h"


// get sockaddr, IPv4 or IPv6:
void * get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


Handler::Handler() {
}


void Handler::handleRequest(char* request, int requestSize, int communicationFD) {

	if (getHost(request)) {

		char const* errorMessage = "Sorry, but the Web page that you were trying to access is inappropriate for you, based on the URL. The page has been blocked to avoid insulting your intelligence.\n\nNet Ninny";

		std::cout<<"detected bad words" << std::endl;
		if (send(communicationFD, errorMessage, strlen(errorMessage), 0) == -1)
            perror("send");errorMessage;
		return;
	}
	std::cout<<"No Bad word detected" << std::endl;
	memset(clientBuff, '0',sizeof(clientBuff));
}

int Handler::getHost(char* request) {

	std::string requestString(request); // This is a copy so that the original does not get affected
	if (checkForBadWords(requestString))
		return 1;
	
	// Get the host name for safe request

	std::istringstream iss(requestString);
    while (std::getline(iss, hostName, '\n'))
    {
        if (hostName.find("host") != std::string::npos) {
        	break;
        }
    }
    hostName.erase (0,5); 
	return 0;
}


int Handler::checkForBadWords(std::string& requestString) {

	std::string badwords[]  = {"spongebob", "britneyspears", "paris hilton", "norrköpping"};

	std::transform(requestString.begin(), requestString.end(), requestString.begin(), ::tolower); // Make everything lowercase
	requestString.erase(std::remove(requestString.begin(), requestString.end(), ' '), requestString.end()); // Remove spaces
	requestString.erase(std::remove(requestString.begin(), requestString.end(), '+'), requestString.end()); // Remove + sign used by google when using spaces

	// Check if we can find any bad words
	for (int i = 0; i < 4; ++i) {
		if (requestString.find(badwords[i]) != std::string::npos)
			return 1;
	}
	return 0;
}


int Handler::StartConnection() {

	memset(&addr, 0, sizeof(addr)); // Empties ?

	addr.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;     // fill in my IP for me


	if ((error = getaddrinfo(hostName.c_str(), "http", &addr, &addrPointer)) != 0) // Get connection information automatically
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

     freeaddrinfo(addrPointer); // all done with this structure

    // If we could not do all three steps with any of the results, then we quit
    if (p == NULL)  {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    return 0;
}

