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
  std::string get_name() { return this->name; }
  std::string op_cli_message();
private:
  Server & server;
  std::vector<Client *> opers;
  std::vector<Client const *> clients;
  std::string name;
};
