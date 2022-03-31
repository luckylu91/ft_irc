#pragma once

#include <netinet/in.h>

#include <cctype>
#include <string>
#include <vector>

class Server;
class Channel;
class Message;

class Client {
 protected:
  Client(Server &server, std::string const &name, std::string const &real_name) :
		server(server), nick(name), user_name(name), real_name(real_name),
		is_identified(true), is_user(true), is_nick(true) {}

 public:
  Client(int sockfd, struct sockaddr_in addr, Server &server);
  virtual ~Client() {}

  static bool invalid_nick(std::string const &nick);

  void set_password(std::string const &password);
  void set_user(std::string const &user_name, std::string const &real_name);
  void set_nick(std::string const &nick);
  bool is_registered() const;
  std::string name() const;
  int get_sockfd() const { return this->sockfd; }
  std::string get_nick() const { return this->nick; }
  std::vector<Channel *> &get_channels() { return this->channels; }
  void add_channel(Channel * channel);
  void remove_channel(Channel const *channel);
  Message base_privmsg() const;
  // void send_message(Client const * dest, std::string const & content) const;
  std::vector<Client const *> related_clients() const;
  virtual void receive_message(Message const &message) const;

 protected:
  int sockfd;
  struct sockaddr_in addr;
  Server &server;
  std::vector<Channel *> channels;
  std::string nick;
  std::string user_name;
  std::string real_name;
  bool is_identified;
  bool is_user;
  bool is_nick;
};
