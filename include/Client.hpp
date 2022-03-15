#pragma once

#include <string>
#include <cctype>
#include <netinet/in.h>
#include <vector>

class Server;
class Channel;
class Message;

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
  std::vector<Channel *> & get_channels() { return this->channels; }
  void remove_channel(Channel const * channel);
  Message base_privmsg() const;
  void send_message(Client const * dest, std::string const & content) const;
  void receive_message(Message const & message) const;

private:
  int sockfd;
  std::vector<Channel *> chan;
  struct sockaddr_in addr;
  Server & server;
  std::vector<Channel *> channels;
  std::string nick;
  std::string user_name;
  std::string real_name;
  bool is_identified;
  bool is_user;
  bool is_nick;
};
