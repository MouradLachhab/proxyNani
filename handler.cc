#include "handler.h"

Handler::Handler() {
}


void Handler::handleRequest(char* request, int requestSize, int communicationFD) {

	if (checkForBadWords(request)) {

		char const* errorMessage = "Sorry, but the Web page that you were trying to access is inappropriate for you, based on the URL. The page has been blocked to avoid insulting your intelligence.\n\nNet Ninny";

		std::cout<<"detected bad words" << std::endl;
		if (send(communicationFD, errorMessage, strlen(errorMessage), 0) == -1)
            perror("send");errorMessage;
		return;
	}
	std::cout<<"No Bad word detected" << std::endl;
	memset(clientBuff, '0',sizeof(clientBuff));
}

int Handler::checkForBadWords(char* request) {


	std::string badwords[]  = {"spongebob", "britneyspears", "paris hilton", "norrköpping"};
	std::string requestString(request); // This is a copy so that the original does not get affected

	std::transform(requestString.begin(), requestString.end(), requestString.begin(), ::tolower); // Make everything lowercase
	for (int i = 0; i < 4; ++i) {
		if (requestString.find(badwords[i]) != std::string::npos)
			return 1;
	}

	return 0;
}