#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void Server::mode_cmd(Client *client, Message const &message) {
  std::vector<std::string> temp_param = message.get_param();
  Channel *channel;
  if (temp_param[0][0] == '#') {
    try {
      channel = find_channel_by_name(temp_param[0]);

      if (message.get_param().size() == 1) {
        return rpl_channelmodeis(client, channel);
      }
      if (!is_in_vector(client, channel->get_operators())) {
        return err_chanoprivsneeded(client, channel);
      }
      channel->mode_cmd_channel(client, message);
    } catch (NoSuchChannelNameException &) {
      this->err_nosuchchannel(client, temp_param[0]);
    } catch (NoSuchClientNickException &) {
      this->err_nosuchnick(client, temp_param[2]);
    }
  }
}
