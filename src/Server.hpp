#pragma once

#include <vector>
#include <string>

class Client;
class Message;

class Server {
public:
  Server(std::string const & name, std::string const & version, std::string const & password);

  void new_client(int sockfd, struct sockaddr_in addr);
  // Client * find_client_by_addr(struct sockaddr_in addr);
  // Client * find_client_by_nick(std::string const & nick);
  bool try_password(std::string const & password) const;
  bool nick_exists(std::string const & nick) const;
  void send_message(Client const *, Message const & message) const;
  void welcome(Client const * client) const;

  Message base_message(std::string const & command);
  Message rpl_welcome(Client * client);
  Message rpl_yourhost();
  Message rpl_created();
  Message rpl_myinfo();
  Message err_needmoreparams(std::string const & command);
  Message err_alreadyregistred();
  Message err_nonicknamegiven();
  Message err_erroneusnickname(std::string const & nick);
  Message err_nicknameinuse(std::string const & nick); //attempt to change to a currently existing nickname.
  Message err_restricted();

private:
  std::string name;
  std::string version;
  std::string password;
  std::vector<Client *> clients;
  time_t cration_time;
};

struct SameNick {
  bool operator()(std::string const & nick, Client const * client) {
    return nick == client->get_nick();
  }
};

struct SameAddr {
  bool operator()(std::string const & nick, Client const * client) {
    return nick == client->get_nick();
  }
};
