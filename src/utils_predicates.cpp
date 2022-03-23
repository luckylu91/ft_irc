#include "utils_predicates.hpp"

#include "Channel.hpp"
#include "Client.hpp"

void SendMessageToClient::operator()(Message const & message, Client const * client) {
	client->receive_message(message);
}

void SendMessageToClientDifferent::operator()(Client const * client) {
	if (client != this->sender)
		client->receive_message(message);
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
}

void RemoveChannelFromClient::operator()(Channel * channel, Client * client) {
	client->remove_channel(channel);
}

