#include "utils.hpp"

#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>

std::string addr_string(struct sockaddr_in addr) {
	std::stringstream ss;
	ss << inet_ntoa(addr.sin_addr);
	return ss.str();
}
