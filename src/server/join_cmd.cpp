#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils.hpp"

void Server::join_cmd(Client *client, Message const &message) {
  std::vector<std::string> args = message.get_param();
  std::vector<std::string> channel_names;

  split(args[0], ',', &channel_names);
  for_each_in_vector(JoinCmd(*this, client), channel_names);
}

void Server::join_cmd_one(Client *client, std::string chan_name) {
  Channel *channel;
  try {
    channel = this->find_channel_by_name(chan_name);
    if (channel->contains_client(client))
      return;
    if (validity_test(client, channel))
      return;
  } catch (NoSuchChannelNameException &) {
    channel = new Channel(*this, chan_name, client);
    channels.push_back(channel);
  }
  client->add_channel(channel);
  channel->add_client(client);
  this->rpl_join(client, channel);
  this->rpl_notopic(client, channel);
  this->rpl_namreply(client, channel);
  this->rpl_endofnames(client, channel);
  this->rpl_channelmodeis(client, channel);
}

int Server::validity_test(Client *client, Channel *channel) {
  if (is_in_vector(client, channel->get_banned_vec())) {
    err_bannedfromchan(client, channel);
    return 1;
  }
  if (channel->get_is_invite_only() && !is_in_vector(client, channel->get_invited_vec())) {
    err_inviteonlychan(client, channel);
    return 1;
  }
  return 0;
}
