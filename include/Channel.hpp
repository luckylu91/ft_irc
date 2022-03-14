#pragma once

class Channel {
public:
  Channel(std::string const & name, Client const * oper);
  int add_client(Client * c);
  std::string get_name();

private:
  Client const * oper;
  std::vector<Client const *> clients;
  std::string name;
};
