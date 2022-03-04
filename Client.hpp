#pragma once
#include <string>
#include <vector>
#include "Channel.hpp"

class Client {
private:
  // address
  std::string host_nickname; // len max: 9
  std::string host_real_name;
  std::string username_on_host;
  std::string server_in_use;
  std::vector<Channel> channels;
};
