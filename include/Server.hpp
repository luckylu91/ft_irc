#pragma once

#include <vector>
#include <string>

class Client;
class Message;
class Channel;

class Server {
public:
  Server(std::string const & name, std::string const & version, std::string const & password);

  void new_client(int sockfd, struct sockaddr_in addr);
  void remove_client(Client const * client);
  void remove_client_sockfd(int sockfd);
  // Client * find_client_by_addr(struct sockaddr_in addr);
  // Client * find_client_by_nick(std::string const & nick);
  Client * find_client_by_sockfd(int sockfd);
  bool try_password(Client const * client, std::string const & password) const;
  bool nick_exists(std::string const & nick) const;
  void send_message(Client const *, Message const & message) const;
  void receive_message(int sockfd, Message const & message);
  void welcome(Client const * client) const;

  Message base_message(std::string const & command) const;
  void rpl_welcome(Client const * client) const;
  void rpl_yourhost(Client const * client) const;
  void rpl_created(Client const * client) const;
  void rpl_myinfo(Client const * client) const;
  void err_needmoreparams(Client const * client, std::string const & command) const;
  void err_alreadyregistred(Client const * client) const;
  void err_nonicknamegiven(Client const * client) const;
  void err_erroneusnickname(Client const * client, std::string const & nick) const;
  void err_nicknameinuse(Client const * client, std::string const & nick) const; //attempt to change to a currently existing nickname.
  void err_restricted(Client const * client) const;
  void err_passwdmismatch(Client const * client) const;
  void err_alreadyregistered(Client const * client) const;


private:
  std::string name;
  std::string version;
  std::string password;
  std::vector<Client *> clients;
  std::vector<Channel *> channels;
  time_t creation_time;
};

struct SameNick {
  bool operator()(std::string const & nick, Client const * client);
};

struct SameSockfd {
  bool operator()(int sockfd, Client const * client);
};

struct RemoveClientFromChannel {
  void operator()(Client * client, Channel * channel);
}


// ERR_BANNEDFROMCHAN
// ERR_INVITEONLYCHAN
// ERR_BADCHANNELKEY
// ERR_CHANNELISFULL
// ERR_BADCHANMASK
// ERR_NOSUCHCHANNEL
// ERR_TOOMANYCHANNELS
// ERR_TOOMANYTARGETS
// ERR_UNAVAILRESOURCE
// RPL_TOPIC
