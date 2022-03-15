#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

typedef std::vector<Client const *>::const_iterator client_iterator;
void Channel::remove_client(Client * client) {
  remove_from_vector(client, this->clients);
  remove_from_vector(client, this->opers);
}

int Channel::add_client(Client * c) {
  if (std::find(clients.begin(), clients.end(),c) == clients.end()) {
    clients.push_back(c);
    c->add_channel(this);
    return 0;
  }
  else {
    std::cout<<"Debug message dans Add_client : Client deja dans le chan";
    return 1;
  }
}

std::string Channel::op_cli_message() const {
	std::string r;

	for(client_iterator it = opers.begin(); it != opers.end();it++)
	{
		r.append("@");
		r.append((*it)->get_nick());
		r.append(" ");
	}
	for(client_iterator it = clients.begin(); it != clients.end();it++)
	{
		r.append((*it)->get_nick());
		r.append(" ");
	}
	return r;
}
