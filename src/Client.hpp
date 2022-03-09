#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include "utils.hpp"

struct Channel;

struct Client {
  Client(struct sockaddr_in addr, std::string nick, std::string real_name, std::string user_name):
    addr(addr),
    nick(nick),
    user_name(user_name),
    real_name(real_name),
    channels()
  {}

  friend bool operator==(Client const & a, Client const & b) {
    return a.nick == b.nick;
  }

  void remove_channel(Channel const & channel);

  std::string client_name() {
    return ::client_name(this->nick, this->user_name, this->addr);
  }

  // address
  struct sockaddr_in addr;
  std::string nick; // len max: 9
  std::string user_name;
  std::string real_name;
  std::vector<Channel const *> channels;
};
