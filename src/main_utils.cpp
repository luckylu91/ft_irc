#include "main_utils.hpp"
#include <signal.h>
#include <unistd.h>
#include<sys/socket.h>
#include <string>
#include <iostream>
#include <vector>
#include "Message.hpp"

extern int sockfd;

void sig_handler(int sig_code) {
	if (sig_code == SIGINT) {
		std::cout << "debug: ^C pressed" << std::endl;
		if (sockfd > 0) {
			std::cout << "debug: socket closed" << std::endl;
			close(sockfd);
			exit(0);
		}
	}
}

void special_print(std::string const & s) {
	std::size_t i;

	for (i = 0; i < s.size(); i++) {
		if (!std::isspace(s[i]) || s[i] == ' ')
			std::cout << s[i];
		else {
			if (s[i] == '\f')
				std::cout << "\\f";
			else if (s[i] == '\n')
				std::cout << "\\n";
			else if (s[i] == '\r')
				std::cout << "\\r";
			else if (s[i] == '\t')
				std::cout << "\\t";
			else if (s[i] == '\v')
				std::cout << "\\v";
		}
	}
	std::cout << std::endl;
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void debug_message_vec(std::vector<Message> & vec) {
	std::cout << "Begin debug message vec" << std::endl;
	for (std::vector<Message>::iterator it = vec.begin(); it != vec.end(); ++it) {
		std::cout << "- message = ";
		special_print(it->to_string());
	}
	std::cout << "End debug message vec" << std::endl;
}
