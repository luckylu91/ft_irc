#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include <unistd.h>

typedef std::vector<Client *>::iterator client_iterator;

Server::Server(std::string const & password): password(password) {}

bool Server::try_password(std::string const & password) const {
  return password == this->password;
}
void Server::send_message(Client const *, std::string const & message_str) const {
  int n = write(client->sockfd, message_str.c_str(), message_str.size());
	if (n < 0)
		throw SocketWriteError();
}

void Server::welcome(Client const *) const;

static struct SameNick {
  bool operator()(std::string const & nick, Client const * client) {
    return nick == client->nick;
  }
};

Server::nick_exists(std::string const & nick) const {
  client_iterator it = find_in_vector<SameNick>(nick, this->clients);
  return (it != this->clients.end());
}
