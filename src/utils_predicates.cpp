#include "utils_predicates.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "errors.hpp"

void SendMessageToClient::operator()(Message const &message, Client const *client) {
  client->receive_message(message);
}

void SendMessageToClientDifferent::operator()(Client const *client) {
  if (client != this->sender)
    client->receive_message(this->message);
}

bool SameNick::operator()(std::string const &nick, Client const *client) {
  return nick == client->get_nick();
}

bool SameSockfd::operator()(int sockfd, Client const *client) {
  return sockfd == client->get_sockfd();
}

bool SameChannelName::operator()(std::string const &name, Channel const *channel) {
  return name == channel->get_name();
}

void RemoveClientFromChannel::operator()(Client *client, Channel *channel) {
  channel->remove_client(client);
}

void RemoveChannelFromClient::operator()(Channel *channel, Client *client) {
  client->remove_channel(channel);
}

void ListCmdNames::operator()(std::string const &channel_name) {
  try {
    Channel *channel = this->server.find_channel_by_name(channel_name);
    this->server.rpl_list(this->client, channel);
  } catch (NoSuchChannelNameException &) {
    this->server.err_nosuchnick(this->client, channel_name);
  }
}

void ListCmdPointers::operator()(Channel const *channel) {
  this->server.rpl_list(this->client, channel);
}

void JoinCmd::operator()(std::string const &channel_name) {
  if (Channel::invalid_channel_name(channel_name))
    return this->server.err_nosuchchannel(this->client, channel_name);
  this->server.join_cmd_one(this->client, channel_name);
}
