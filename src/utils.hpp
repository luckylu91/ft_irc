#pragma once

#include "utils_generic.tpp"
#include <string>
#include <vector>

struct Client;
struct Channel;

std::vector<Channel>::iterator find_channel_by_name(std::string const & name, std::vector<Channel> & channels);
std::vector<Channel>::const_iterator find_channel_by_name(std::string const & name, std::vector<Channel> const & channels);
bool channel_name_in_list(std::string const & name, std::vector<Channel> const & channels);

void add_client_to_channel(Client & client, Channel & channel);

