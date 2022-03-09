#pragma once
#include <string>
#include <vector>
#include "utils.hpp"

struct Client;

struct Channel {
  Channel(std::string name, Client * oper):
    name(name),
    oper(oper),
    clients(),
    is_private(false),
    is_secret(false),
    is_invite_only(false),
    is_topic_settable(false),
    is_unqueriable_by_outside_client(false),
    is_moderated(false),
    limit(-1),
    // banmask
    password() {
    add_client_to_channel(oper, this);
  }

  friend bool operator==(Channel const & a, Channel const & b) {
    return a.name == b.name;
  }

  size_t size() { return clients.size(); }

  void remove_client(Client const & client);

  std::string name;    // max len: 50; begins with '&', '#', '+' or '!'; no space, ^G or ','
  Client const * oper; // channel operator, has leading '@'
  std::vector<Client const *> clients;
  bool is_private;
  bool is_secret;
  bool is_invite_only;
  bool is_topic_settable;
  bool is_unqueriable_by_outside_client;
  bool is_moderated;
  int limit;
  // banmask
  std::string password;
};

