#include "utils.hpp"

#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>

std::string addr_string(struct sockaddr_in addr) {
	std::stringstream ss;
	ss << inet_ntoa(addr.sin_addr);
	return ss.str();
}

std::string special_string(std::string const & s) {
	std::size_t i;
	std::stringstream ss;

	for (i = 0; i < s.size(); i++) {
		if (!std::isspace(s[i]) || s[i] == ' ')
			ss << s[i];
		else {
			if (s[i] == '\f')
				ss << "\\f";
			else if (s[i] == '\n')
				ss << "\\n";
			else if (s[i] == '\r')
				ss << "\\r";
			else if (s[i] == '\t')
				ss << "\\t";
			else if (s[i] == '\v')
				ss << "\\v";
		}
	}
	return ss.str();
}

