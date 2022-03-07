#pragma once
#include <string>
#include <vector>
#include "Client.hpp"

struct Channel {
  Channel(std::string name, Client const * & oper):
    name(name),
    oper(oper),
    clients(1, oper),
    isPrivate(false),
    isSecret(false),
    isInviteOnly(false),
    isTopicSettable(false),
    isUnqueriableByOutsideClient(false),
    isModerated(false),
    limit(-1),
    // banmask
    password()
  {}

  std::string name;  // max len: 200; begins with '&'|'#'; no space, ^G or ','
  Client const * oper; // channel operator, has leading '@'
  std::vector<Client const *> clients;
  bool isPrivate;
  bool isSecret;
  bool isInviteOnly;
  bool isTopicSettable;
  bool isUnqueriableByOutsideClient;
  bool isModerated;
  int limit = -1;
  // banmask
  std::string password;
};

