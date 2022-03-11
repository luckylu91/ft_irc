#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "numeric_codes.hpp"
#include <unistd.h>
#include <ctime>

typedef std::vector<Client *>::iterator client_iterator;

Server::Server(std::string const & name, std::string const & version, std::string const & password):
  name(name),
  version(version),
  password(password),
  creation_time(time(0)) {}


// Client * find_client_by_addr(struct sockaddr_in addr) {

// }

// Client * find_client_by_nick(std::string const & nick) {

// }

void new_client(int sockfd, struct sockaddr_in addr) {
  Client * client = new Client(sockfd, addr, *this);
  this->clients.push_back(client);
}

bool Server::try_password(std::string const & password) const {
  return password == this->password;
}

void Server::send_message(Client const * client, Message const & message) const {
  std::string message_str = message.to_string();
  int n = write(client->get_sockfd(), message_str.c_str(), message_str.size());
	if (n < 0)
		throw ClientSocketWriteException(client);
}

void Server::welcome(Client const * client) const {
  this->send
}

bool Server::nick_exists(std::string const & nick) const {
  client_iterator it = find_in_vector<SameNick>(nick, this->clients);
  return (it != this->clients.end());
}


Message Server::base_message(std::string const & command) {
    Message message;

  message.set_source(this->name);
  message.set_command(command);
  return message;
}

Message rpl_welcome(Client * client) {
  Message m = this->base_message(RPL_WELCOME);

  m.add_param("Welcome to the Internet Relay Network" + client->name());
  return m;
}
Message rpl_yourhost() {
  Message m = this->base_message(RPL_YOURHOST);
  m.add_param("Your host is" + this->name + "running version" + this->version);
  return m;
}
Message rpl_created() {
  Message m = this->base_message(RPL_CREATED);
  m.add_param("This server was created" + std:string(ctime(&this->cration_time)));
  m.add_param("Not enough parameters");
  return m;
}
Message rpl_myinfo() {
  Message m = this->base_message(RPL_MYINFO);
  m.add_param(this->name + " " + this->version); // + "<available user modes> <available channel modes>");
  return m;
}

Message Server::err_needmoreparams(std::string const & command) {
  Message m = this->base_message(ERR_NEEDMOREPARAMS);
  m.add_param(command);
  m.add_param("Not enough parameters");
  return m;
}
Message Server::err_alreadyregistred() {
  Message m = this->base_message(ERR_ALREADYREGISTRED);
  m.add_param("Unauthorized command (already registered)");
  return m;
}
Message Server::err_nonicknamegiven() {
  Message m = this->base_message(ERR_NONICKNAMEGIVEN);
  m.add_param("No nickname given");
  return m;
}
Message Server::err_erroneusnickname(std::string const & nick) {
  Message m = this->base_message(ERR_ERRONEUSNICKNAME);
  m.add_param(nick);
  m.add_param("Erroneous nickname");
  return m;
}
Message Server::err_nicknameinuse(std::string const & nick) {
  Message m = this->base_message(ERR_NICKNAMEINUSE);
  m.add_param("Nickname is already in use");
  return m;
}
Message Server::err_restricted() {
  Message m = this->base_message(ERR_RESTRICTED);
  m.add_param("Your connection is restricted!");
  return m;
}
