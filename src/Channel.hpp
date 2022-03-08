#pragma once
#include <string>
#include <vector>
#include "utils.hpp"

struct Client;

struct Channel {
  Channel(std::string name, Client * oper):
    name(name),
    oper(NULL),
    clients(),
    isPrivate(false),
    isSecret(false),
    isInviteOnly(false),
    isTopicSettable(false),
    isUnqueriableByOutsideClient(false),
    isModerated(false),
    limit(-1),
    // banmask
    password()
  {
    add_client_to_channel(*oper, *this);
  }

  friend bool operator==(Channel const & a, Channel const & b) {
    return a.name == b.name;
  }

  size_t size() { return clients.size(); }

  void remove_client(Client const & client);

  std::string name;    // max len: 200; begins with '&'|'#'; no space, ^G or ','
  Client const * oper; // channel operator, has leading '@'
  std::vector<Client const *> clients;
  bool isPrivate;
  bool isSecret;
  bool isInviteOnly;
  bool isTopicSettable;
  bool isUnqueriableByOutsideClient;
  bool isModerated;
  int limit;
  // banmask
  std::string password;
};

