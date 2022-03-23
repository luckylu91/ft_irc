#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

/*
	Channels names are strings (beginning with a '&' or '#' character) of
	length up to 200 characters.  Apart from the the requirement that the
	first character being either '&' or '#'; the only restriction on a
	channel name is that it may not contain any spaces (' '), a control G
	(^G or ASCII 7), or a comma (',' which is used as a list item
	separator by the protocol).
*/

bool Channel::invalid_channel_name(std::string const & name) {
	char illegal_chars[] = {
		' ', 7, ',', '\0'
	};
	if (name.find_first_of(illegal_chars) != std::string::npos)
		return true;
	if (name[0] != '#' && name[0] != '&')
		return true;
	if (name.size() > 200)
		return true;
	return false;
}

typedef std::vector<Client const *>::const_iterator client_iterator;
void Channel::remove_client(Client * client) {
	remove_from_vector(client, this->clients);
	remove_from_vector(client, this->opers);
	if (this->size() == 0)
		this->server.remove_channel(this);
}

std::string Channel::op_cli_message() const {
	std::string r;

	for(client_iterator it = clients.begin(); it != clients.end();it++)
	{
		if (it != clients.begin())
			r.append(" ");
		r.append((*it)->get_nick());
	}
	if (clients.size() > 0 && opers.size() > 0)
		r.append(" ");
	for(client_iterator it = opers.begin(); it != opers.end();it++)
	{
		if (it != opers.begin())
			r.append(" ");
		r.append("@");
		r.append((*it)->get_nick());
	}
	return r;
}

void Channel::forward_message(Client const * src, std::string const & content) const {
	Message message;
	message.set_source(src->name());
	message.add_param(this->get_name());
	message.add_param(content);
	for_each_in_vector<SendMessageToClient>(message, this->clients);
	for_each_in_vector<SendMessageToClient>(message, this->opers);
}

void SendMessageToClient::operator()(Message const & message, Client const * client) {
	client->receive_message(message);
}

bool Channel::contains_client(Client const * client) const {
	return is_in_vector(client, this->opers) || is_in_vector(client, this->clients);
}

bool Channel::is_operator(Client const * client) const {
	return is_in_vector(client, this->opers);
}

bool Channel::is_normal_user(Client const * client) const {
	return is_in_vector(client, this->clients);
}

std::size_t Channel::size() const {
	return this->clients.size() + this->opers.size();
}
