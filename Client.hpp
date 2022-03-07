#pragma once
#include <string>
#include <vector>
#include "Channel.hpp"

struct Client {
  Client(std::string nick, std::string real_name, std::string user_name):
    host_nickname(host_nickname),
    host_real_name(host_real_name),
    username_on_host(username_on_host),
    server_in_use(server_in_use),
    channels()
  {}


  friend bool operator==(Client const & a, Client const & b) {
    return a.host_nickname == b.host_nickname;
  }

  // address
  std::string host_nickname; // len max: 9
  std::string host_real_name;
  std::string username_on_host;
  std::string server_in_use;
  std::vector<Channel const *> channels;
};
