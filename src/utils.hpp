#pragma once

#include "utils_generic.tpp"
#include <netinet/in.h>
#include <string>
#include <vector>

struct Client;
struct Channel;

std::vector<Channel>::iterator find_channel_by_name(std::string const & name, std::vector<Channel> & channels);
std::vector<Channel>::const_iterator find_channel_by_name(std::string const & name, std::vector<Channel> const & channels);
bool channel_name_in_list(std::string const & name, std::vector<Channel> const & channels);
void add_client_to_channel(Client const * client, Channel const * channel);
std::string addr_string(struct socketaddr_in addr);
std::string client_name(std::string const & nick, std::string const & user_name, struct socketaddr_in addr);

