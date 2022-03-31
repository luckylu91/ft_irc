#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void Server::topic_cmd(Client *client, Message const &message) {
  std::vector<std::string> temp_param = message.get_param();
  Channel *channel;
  try {
    channel = find_channel_by_name(temp_param[0]);
    if (!channel->contains_client(client)) {
      return err_notonchannel(client, channel);
    }
    if (message.get_param().size() == 1) {
      if ((channel->get_topic()).empty())
        return rpl_notopic(client, channel);
      else
        return rpl_topic(client, channel);
    }
    if (channel->get_is_topic_protected() && !channel->is_operator(client)) {
      err_chanoprivsneeded(client, channel);
      return;
    }
    channel->set_topic(temp_param[1]);
    this->rpl_topic_set(client, channel);
  } catch (NoSuchChannelNameException &) {
    this->err_nosuchchannel(client, temp_param[0]);
  }
}
