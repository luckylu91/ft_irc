#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "numeric_codes.hpp"
#include "utils_predicates.hpp"
#include "utils_template.tpp"

void Server::welcome(Client const* client) const {
  this->rpl_welcome(client);
  this->rpl_yourhost(client);
  this->rpl_created(client);
  this->rpl_myinfo(client);
}

void Server::rpl_welcome(Client const* client) const {
  Message m = this->base_message(client, RPL_WELCOME);
  m.add_param("Welcome to the Internet Relay Network " + client->name());
  this->send_message(client, m);
}
void Server::rpl_yourhost(Client const* client) const {
  Message m = this->base_message(client, RPL_YOURHOST);
  m.add_param("Your host is " + this->name + " running version " + this->version);
  this->send_message(client, m);
}
void Server::rpl_created(Client const* client) const {
  Message m = this->base_message(client, RPL_CREATED);
  m.add_param("This server was created " + this->creation_time_string);
  this->send_message(client, m);
}
void Server::rpl_myinfo(Client const* client) const {
  Message m = this->base_message(client, RPL_MYINFO);
  m.add_param(this->name + " " + this->version);  // + "<available user modes> <available channel modes>");
  this->send_message(client, m);
}

// RPL JOIN

void Server::rpl_join(Client const* client, Channel const* chan) const {
  Message m;
  m.set_source(client->name());
  m.set_command("JOIN");
  m.add_param(chan->get_name());
  chan->forward_message(m);
}

void Server::rpl_namreply(Client const* client, Channel const* chan) const {
  Message m = this->base_message(client, RPL_NAMREPLY);
  m.add_param("@");
  m.add_param(chan->get_name());
  m.add_param(chan->op_cli_message());
  this->send_message(client, m);
}

void Server::rpl_endofnames(Client const* client, Channel const* chan) const {
  Message m = this->base_message(client, RPL_ENDOFNAMES);
  m.add_param(chan->get_name());
  m.add_param("End of NAMES list");
  this->send_message(client, m);
}

// RPL PONG

void Server::rpl_pong(Client const* client) const {
  Message m = this->base_message_no_nick("PONG");
  m.add_param(this->name);
  this->send_message(client, m);
}
// RPL INVITING

void Server::rpl_inviting(Client const* client, Channel const* channel, Client const* target) const {
  Message m = this->base_message(client, RPL_INVITING);
  m.add_param(target->get_nick());
  m.add_param(channel->get_name());
  this->send_message(client, m);
}
// PART & KICK

void Server::rpl_part_and_remove(Client* client, Channel* channel, std::string const& part_message) const {
  Message m;
  m.set_source(client->name());
  m.set_command("PART");
  m.add_param(channel->get_name());
  m.add_param(part_message);
  channel->forward_message(m);
  channel->remove_client(client);
}

void Server::rpl_kick_and_remove(Client const* src, Client* dest, Channel* channel, std::string const& kick_message) const {
  Message m;
  m.set_source(src->name());
  m.set_command("KICK");
  m.add_param(channel->get_name());
  m.add_param(dest->name());
  m.add_param(kick_message);
  channel->forward_message(m);
  channel->remove_client(dest);
}

// QUIT

void Server::rpl_quit(Client const* client, std::string const& quit_msg) {
  std::cout << std::endl;
  Message m;
  m.set_source(client->name());
  m.set_command("QUIT");
  if (quit_msg.size() > 0)
    m.add_param(quit_msg);
  else
    m.add_param("Client Quit");
  for_each_in_vector(SendMessageToClientDifferent(client, m), client->related_clients());
}
// TOPIC

void Server::rpl_notopic(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, RPL_NOTOPIC);
  m.add_param(channel->get_name());
  m.add_param("No topic is set");
  this->send_message(client, m);
}

void Server::rpl_topic(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, RPL_TOPIC);
  m.add_param(channel->get_name());
  m.add_param(channel->get_topic());
  this->send_message(client, m);
}

// LIST

void Server::rpl_list(Client const* client, Channel const* channel) const {
  if (channel->get_is_private() || channel->get_is_secret())
    return;
  Message m = this->base_message(client, RPL_LIST);
  m.add_param(channel->get_name());
  m.add_param(channel->size_str());
  // m.add_param(channel->topic());
  this->send_message(client, m);
}

void Server::rpl_listend(Client const* client) const {
  Message m = this->base_message(client, RPL_LISTEND);
  m.add_param("End of LIST");
  this->send_message(client, m);
}
// MODE
// 324 	RPL_CHANNELMODEIS 	RFC1459 	<channel> <mode> <mode_params>
void Server::rpl_channelmodeis(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, RPL_CHANNELMODEIS);
  m.add_param(channel->get_name());
  m.add_param(channel->mode_to_string());
  this->send_message(client, m);
}
// NICK

void Server::rpl_nick(Client const* client, std::string const nick) {
  Message m;
  m.set_source(client->name());
  m.set_command("NICK");
  m.add_param(nick);
  client->receive_message(m);
  for_each_in_vector(SendMessageToClientDifferent(client, m), client->related_clients());
}

void Server::rpl_topic_set(Client const* client, Channel const* channel) const {
  Message m;
  m.set_source(client->name());
  m.set_command("TOPIC");
  m.add_param(channel->get_name());
  m.add_param(channel->get_topic());
  this->send_message(client, m);
}

void Server::rpl_banlist(Client const* client, Channel const* channel, std::string const nick) {
  Message m = this->base_message(client, RPL_BANLIST);
  m.add_param(channel->get_name());
  m.add_param(nick);
  this->send_message(client, m);
}

void Server::rpl_endofbanlist(Client const* client, Channel const* channel) {
  Message m = this->base_message(client, RPL_ENDOFBANLIST);
  m.add_param(channel->get_name());
  m.add_param(":End of channel ban list");
  this->send_message(client, m);
}
