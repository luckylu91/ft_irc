#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void Server::invite_cmd(Client *client, Message const &message) {
  std::vector<std::string> temp_param = message.get_param();
  Channel *channel;
  try {
    channel = this->find_channel_by_name(temp_param[1]);
    channel->invite_cmd_channel(client, temp_param);
  } catch (NoSuchClientNickException &) {
    this->err_nosuchnick(client, temp_param[2]);
  }
	catch (NoSuchChannelNameException &) {
    this->err_nosuchchannel(client, temp_param[1]);
  }
}
