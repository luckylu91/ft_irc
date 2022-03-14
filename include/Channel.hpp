#pragma once

#include <string>
#include <vector>

class Server;
class Client;

class Channel {
public:
  Channel(std::string const & name, Client const * oper);

  void remove_client(Client * client);
  int add_client(Client * c);
  std::string get_name() const { return this->name; }

private:
  Server & server;
  std::vector<Client *> opers;
  std::vector<Client *> clients;
  std::string name;
};
