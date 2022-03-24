#pragma once

#include <string>
#include <vector>
#include "utils_template.tpp"
#include "utils_predicates.hpp"
#include <netinet/in.h>

std::string addr_string(struct sockaddr_in addr);
std::string special_string(std::string const & s);
std::string split_return_remaining(std::string const & buffer, char sep, std::vector<std::string> * result_vec);
void split(std::string const & buffer, char sep, std::vector<std::string> * result_vec);
