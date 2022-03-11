#pragma once

#include "utils_template.tpp"
#include <netinet/in.h>

std::string addr_string(struct sockaddr_in addr);
