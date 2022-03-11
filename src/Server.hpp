#pragma once

#include <vector>
#include <string>

class Client;
class Message;

class Server {
public:
  Server(std::string const & password);

  bool try_password(std::string const & password) const;
  bool nick_exists(std::string const & nick) const;
  void send_message(Client const *, Message const & message) const;
  void welcome(Client const *) const;

private:
  std::string password;
  std::vector<Client *> clients;
};
