#pragma once

#include <vector>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include "utils.hpp"

struct Server {
  Server(): channels() {}

  Channel & add_client_to_channel(Client & client, std::string channel_name);
  void remove_client_from_channel(Client & client, std::string channel_name);

  std::vector<Channel> channels;
  std::string password;
};

