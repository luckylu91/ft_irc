#pragma once

#include <string>
#include <cctype>
#include <netinet/in.h>
#include "Server.hpp"
#include "Channel.hpp"
class Server;

class Client {
public:
  Client(int sockfd, struct sockaddr_in addr, Server & server);

  void set_password(std::string const & password);
  void set_user(std::string const & user_name, std::string const & real_name);
  void set_nick(std::string const & nick);
  bool is_registered() const;
  std::string name() const;
  int get_sockfd() const { return this->sockfd; }
  std::string get_nick() const { return this->nick; }
  void  add_chan(Channel * chan);

private:
  int sockfd;
  std::vector<Channel *> chan;
  struct sockaddr_in addr;
  Server & server;
  std::string nick;
  std::string user_name;
  std::string real_name;
  bool is_identified;
  bool is_user;
  bool is_nick;
};
