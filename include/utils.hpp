#pragma once

#include "utils_template.tpp"
#include <netinet/in.h>

std::string addr_string(struct sockaddr_in addr);
std::size_t ft_strlen(char const * s);
