#include "Client.hpp"

#include <unistd.h>

#include <iostream>
#include <sstream>

#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils.hpp"

// special = "[", "]", "\", "`", "_", "^", "{", "|", "}"
// letter = A-Z / a-z
// nickname = ( letter / special ) *8( letter / digit / special / "-" )

typedef std::vector<Channel *>::const_iterator channel_const_iterator;

static bool is_special_char(char c) {
  switch (c) {
    case '[':
    case ']':
    case '\\':
    case '`':
    case '_':
    case '^':
    case '{':
    case '|':
    case '}':
      return true;
    default:
      return false;
  }
}

bool Client::invalid_nick(std::string const &nick) {
  if (nick.size() == 0 || nick.size() > 9)
    return true;
  if (!std::isalpha(nick[0]) && !is_special_char(nick[0]))
    return true;
  for (size_t i = 1; i < nick.size(); i++) {
    if (!std::isalnum(nick[i]) && !is_special_char(nick[i]) && nick[i] != '-')
      return true;
  }
  return false;
}

Client::Client(int sockfd, struct sockaddr_in addr, Server &server) :
  sockfd(sockfd),
  addr(addr),
  server(server),
  nick("*"),
  user_name("*"),
  real_name("*"),
  is_identified(false),
  is_user(false),
  is_nick(false) {}

void Client::set_password(std::string const &password) {
  if (this->is_registered()) {
    return this->server.err_alreadyregistred(this);
  }
  if (this->server.try_password(password)) {
    this->is_identified = true;
  } else {
    this->is_identified = false;
    return this->server.err_passwdmismatch(this);
  }
}

void Client::set_user(std::string const &user_name, std::string const &real_name) {
  if (!this->is_identified && this->is_nick) {
    return this->server.err_passwdmismatch(this);
  }
  if (this->is_registered()) {
    return this->server.err_alreadyregistred(this);
  }
  this->user_name = user_name;
  this->real_name = real_name;
  if (!this->is_user && this->is_nick) {
    this->server.welcome(this);
  }
  this->is_user = true;
}

// (before)
// ERR_NONICKNAMEGIVEN (431)
void Client::set_nick(std::string const &nick) {
  if (!this->is_identified && this->is_user) {
    return this->server.err_passwdmismatch(this);
  }
  if (invalid_nick(nick)) {
    return this->server.err_erroneusnickname(this, nick);
  }
  if (this->server.nick_exists(nick)) {
    return this->server.err_nicknameinuse(this, nick);
  }
  if (this->is_registered())
    server.rpl_nick(this, nick);
  this->nick = nick;
  if (!this->is_nick && this->is_user) {
    this->server.welcome(this);
  }
  this->is_nick = true;
}

bool Client::is_registered() const {
  return this->is_identified && this->is_user && this->is_nick;
}

std::string Client::name() const {
  std::stringstream ss;

  ss << this->nick;
  ss << "!" << this->user_name;
  ss << "@" << addr_string(this->addr);
  return ss.str();
}

void Client::add_channel(Channel *channel) {
  add_if_no_in(channel, this->channels);
}

void Client::remove_channel(Channel const *channel) {
  remove_from_vector(channel, this->channels);
}

Message Client::base_privmsg() const {
  Message message;
  message.set_source(this->name());
  message.set_command("PRIVMSG");
  return message;
}

void Client::receive_message(Message const &message) const {
  (this->server.get_io_manager())->send_message(this->sockfd, message);
}

std::vector<Client const *> Client::related_clients() const {
  std::vector<Client const *> related_clients;
  for (std::size_t i = 0; i < this->channels.size(); i++) {
    std::vector<Client const *> channel_clients = this->channels[i]->get_clients();
    for (std::size_t j = 0; j < channel_clients.size(); j++) {
      add_if_no_in(channel_clients[j], related_clients);
    }
  }
  return related_clients;
}
