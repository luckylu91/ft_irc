#include "Client.hpp"

Client::Client(int sockfd, struct sockaddr_in addr, Server & server):
  sockfd(sockfd),
  addr(addr),
  server(server),
  nick("no_nick"),
  user_name("no_user_name"),
  real_name("no_real_name") {}

void Client::set_password(std::string const & password) {
  if (this->is_registered()) {
    // ERR_ALREADYREGISTRED (462)
  }
  if (this->server.try_password(password)) {
    this->is_identified = true;
  }
  else {
    //  ERR_PASSWDMISMATCH (464)
    this->is_identified = false;
  }
}

void Client::set_user(std::string const & user_name, std::string const & real_name) {
  if (this->is_registered()) {
    // ERR_ALREADYREGISTERED (462)
  }
  this->user_name = user_name;
  this->real_name = real_name;
  if (!this->is_user && this->is_identified && this->is_nick) {
    this->server.welcome(this);
  }
  this->is_user = true;
}

// (before)
// ERR_NONICKNAMEGIVEN (431)
void Client::set_nick(std::string const & nick) {
  if (invalid_nick(nick)) {
    // ERR_ERRONEUSNICKNAME (432)
  }
  if (this->server.nick_exists(nick)) {
    // ERR_NICKNAMEINUSE (433)
  }
  this->nick = nick;
  if (!this->is_nick && this->is_identified && this->is_user) {
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
  ss << "@" << addr_string(this->sockfd);
  return ss.str();
}


// special = "[", "]", "\", "`", "_", "^", "{", "|", "}"
// letter = A-Z / a-z
// nickname = ( letter / special ) *8( letter / digit / special / "-" )

static bool is_special_char(char c) {
  return c == '[' || c ==  ']' || c ==  '\\' || c ==  '`' || c ==  '_'
    || c ==  '^' || c ==  '{' || c ==  '|' || c ==  '}';
}

static bool invalid_nick(std::string const & nick) {
  if (nick.size() == 0 || nick.size() > 9)
    return true;
  if (!std::isalpha(nick[0]) && !is_special_char(nick[0]))
    return true;
  for (size_t i = 0; i < nick.length(); i++) {
    if (!std::isalnum(nick[0]) && !is_special_char(nick[0]) && nick[i] != '-')
      return true;
  }
  return false;
}
