#pragma once
#include <string>
#include <vector>

struct Channel;

struct Client {
  Client(std::string nick, std::string real_name, std::string username):
    nick(nick),
    real_name(real_name),
    username(username),
    channels()
  {}

  friend bool operator==(Client const & a, Client const & b) {
    return a.nick == b.nick;
  }

  void remove_channel(Channel const & channel);

  // address
  std::string nick; // len max: 9
  std::string real_name;
  std::string username;
  std::vector<Channel const *> channels;
};
