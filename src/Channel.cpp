#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

typedef std::vector<Client const *>::const_iterator client_iterator;
void Channel::remove_client(Client * client) {
  remove_from_vector(client, this->clients);
  remove_from_vector(client, this->opers);
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

void Channel::forward_message(Client const * src, std::string const & content) const {
  Message message;
  message.set_source(src->name());
  message.add_param(this->get_name());
  message.add_param(content);
  // for_each_in_vector<SendMessageToClient>
}

void SendMessageToClient::operator()(Message const & message, Client const * client) {
  client->receive_message(message);
}
