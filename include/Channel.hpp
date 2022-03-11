#pragma once

class Channel {
public:
  Channel(std::string const & name, Client const * oper);



private:
  Client const * oper;
  std::vector<Client const *> clients;
}
