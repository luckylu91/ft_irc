#pragma once

#include <string>
#include <vector>

class Server;
class Client;

class Channel {
public:
  Channel(std::string const & name, Client const * oper);

  void remove_client(Client * client);

private:
  Server & server;
  std::vector<Client *> opers;
  std::vector<Client *> clients;
};
