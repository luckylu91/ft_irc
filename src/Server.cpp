#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "Channel.hpp"
#include "numeric_codes.hpp"
#include <unistd.h>
#include <ctime>

typedef std::vector<Client *>::const_iterator client_iterator;
typedef std::vector<Channel *>::const_iterator channel_iterator;

Server::Server(std::string const & name, std::string const & version, std::string const & password):
  name(name),
  version(version),
  password(password),
  clients(),
  creation_time(time(0)) {}


Client * Server::find_client_by_sockfd(int sockfd) {
  return *find_in_vector<SameSockfd>(sockfd, this->clients);
}

Client * Server::find_client_by_nick(std::string const & nick) {
  client_iterator it = find_in_vector<SameNick>(nick, this->clients);
  if (it != this->clients.end())
    return *it;
  else
    return NULL;
}

Channel * Server::find_channel_by_name(std::string const & name) {
  channel_iterator it = find_in_vector<SameChannelName>(name, this->channels);
  if (it != this->channels.end())
    return *it;
  else
    return NULL;
}

// Client * find_client_by_addr(struct sockaddr_in addr) {

// }

// Client * find_client_by_nick(std::string const & nick) {

// }

void Server::new_client(int sockfd, struct sockaddr_in addr) {
  Client * client = new Client(sockfd, addr, *this);
  this->clients.push_back(client);
}

void Server::remove_client(Client const * client) {
  for_each_in_vector<RemoveClientFromChannel>(client, this->channels);
}

void Server::remove_client_sockfd(int sockfd) {
  client_iterator it = find_in_vector<SameSockfd>(sockfd, this->clients);
  this->clients.erase(it);
}

bool Server::try_password(Client const * client, std::string const & password) const {
  return password == this->password;
}

void Server::send_message(Client const * client, Message const & message) const {
  std::string message_str = message.to_string();
  std::cout<<"debug dans cliend.cpp send message\n message = "<<message.to_string()<<std::endl;
  int n = write(client->get_sockfd(), message_str.c_str(), message_str.size());
  std::cout<<"debug dans send message string = "<<message_str.c_str()<<" n ="<<n<<std::endl;
	if (n < 0)
		throw ClientSocketWriteException(client);
}

void Server::receive_message(int sockfd, Message const & message) {
  Client * client = this->find_client_by_sockfd(sockfd);
  if (message.get_command() == "NICK") {
    if (message.get_param().size() == 0)
      return this->err_nonicknamegiven(client);
	  std::cout<<"debug dans recerive avant set_nic\n";
    client->set_nick(message.get_param()[0]);
  }
  else if (message.get_command() == "USER") {
    if (message.get_param().size() < 4)
      return this->err_needmoreparams(client, "USER");
    client->set_user(message.get_param()[0], message.get_param()[3]);
  }
  else if (message.get_command() == "PASS") {
    if (message.get_param().size() == 0)
      return this->err_needmoreparams(client, "PASS");
    client->set_password(message.get_param()[0]);
  }

  else if (message.get_command() == "JOIN") {

  }
  else if (message.get_command() == "PRIVMSG") {
    if (message.get_param().size() == 0)
      return this->err_norecipient(client, "PRIVMSG");
    if (message.get_param().size() == 1)
      return this->err_notexttosend(client, "PRIVMSG");
    this->privmsg(client, message.get_param()[0], message.get_param()[1]);
  }
}

// ERR_NORECIPIENT
// ERR_NOTEXTTOSEND
// ERR_CANNOTSENDTOCHAN
// ERR_NOTOPLEVEL
// ERR_WILDTOPLEVEL
// ERR_TOOMANYTARGETS
// ERR_NOSUCHNICK
// RPL_AWAY
void Server::privmsg(Client const * src, std::string const & msgtarget, std::string message) const {
  Client * dest_client = this->find_client_by_nick(msgtarget);
  if (dest_client != NULL) {
    src->send_message(dest_client, message);
    return ;
  }
  Channel * dest_channel = this->find_channel_by_name(msgtarget);
  if (dest_channel != NULL) {
    this->msg_channel(src, dest_channel, message);
    return ;
  }
  this->err_nosuchnick(src, msgtarget);
}

void Server::welcome(Client const * client) const {
  this->rpl_welcome(client);
  this->rpl_yourhost(client);
  this->rpl_created(client);
  this->rpl_myinfo(client);
}

bool Server::nick_exists(std::string const & nick) const {
  client_iterator it = find_in_vector<SameNick>(nick, this->clients);
  return (it != this->clients.end());
}


Message Server::base_message(std::string const & command) const {
  Message message;
  message.set_source(this->name);
  message.set_command(command);
  return message;
}

void Server::rpl_welcome(Client const * client) const {
  Message m = this->base_message(RPL_WELCOME);

  m.add_param("Welcome to the Internet Relay Network " + client->name());
  this->send_message(client, m);
}
void Server::rpl_yourhost(Client const * client) const {
  Message m = this->base_message(RPL_YOURHOST);
  m.add_param("Your host is " + this->name + " running version " + this->version);
  this->send_message(client, m);
}
void Server::rpl_created(Client const * client) const {
  Message m = this->base_message(RPL_CREATED);
  m.add_param("This server was created " + std::string(ctime(&this->creation_time)));
  this->send_message(client, m);
}
void Server::rpl_myinfo(Client const * client) const {
  Message m = this->base_message(RPL_MYINFO);
  m.add_param(this->name + " " + this->version); // + "<available user modes> <available channel modes>");
  this->send_message(client, m);
}

void Server::err_needmoreparams(Client const * client, std::string const & command) const {
  Message m = this->base_message(ERR_NEEDMOREPARAMS);
  m.add_param(command);
  m.add_param("Not enough parameters");
  this->send_message(client, m);
}
void Server::err_alreadyregistred(Client const * client) const {
  Message m = this->base_message(ERR_ALREADYREGISTRED);
  m.add_param("Unauthorized command (already registered)");
  this->send_message(client, m);
}
void Server::err_nonicknamegiven(Client const * client) const {
  Message m = this->base_message(ERR_NONICKNAMEGIVEN);
  m.add_param("No nickname given");
  this->send_message(client, m);
}
void Server::err_erroneusnickname(Client const * client, std::string const & nick) const {
  Message m = this->base_message(ERR_ERRONEUSNICKNAME);
  m.add_param(nick);
  m.add_param("Erroneous nickname");
  this->send_message(client, m);
}
void Server::err_nicknameinuse(Client const * client, std::string const & nick) const {
  Message m = this->base_message(ERR_NICKNAMEINUSE);
  m.add_param("Nickname is already in use");
  this->send_message(client, m);
}
void Server::err_restricted(Client const * client) const {
  Message m = this->base_message(ERR_RESTRICTED);
  m.add_param("Your connection is restricted!");
  this->send_message(client, m);
}
void Server::err_passwdmismatch(Client const * client) const {
  Message m = this->base_message(ERR_PASSWDMISMATCH);
  m.add_param(client->name());
  m.add_param("Password incorrect");
  this->send_message(client, m);
}
void Server::err_nosuchnick(Client const * client, std::string const & nick) const {
  Message m = this->base_message(ERR_NOSUCHNICK);
  m.add_param(nick);
  m.add_param("No such nick/channel");
  this->send_message(client, m);
}

void Server::join_cmd(Client * c, std::string chan_name)
{
  for(std::vector<Channel *>::iterator it = channels.begin(); it != channels.end();it++)
  {
    if ((*it)->get_name() == chan_name)
    {
      (*it)->add_client(c);
      return;
    }
  }
  channels.push_back(new Channel(chan_name,c));
}

bool SameNick::operator()(std::string const & nick, Client const * client) {
  return nick == client->get_nick();
}

bool SameSockfd::operator()(int sockfd, Client const * client) {
  return sockfd == client->get_sockfd();
}

bool SameChannelName::operator()(std::string const & name, Channel const * channel) {
  return name == channel->get_name();
}

void RemoveClientFromChannel::operator()(Client * client, Channel * channel) {
  channel->remove_client(client);
  client->remove_channel(channel);
}

