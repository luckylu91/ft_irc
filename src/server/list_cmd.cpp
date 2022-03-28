#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "numeric_codes.hpp"
#include "utils.hpp"

void Server::list_cmd(Client const *client, Message const &message) const {
  std::vector<std::string> args = message.get_param();
  std::vector<std::string> channel_names;
  std::vector<Channel *> channels;

  if (args.size() == 0)
    for_each_in_vector(ListCmdPointers(*this, client), this->channels);
  else {
    split(args[0], ',', &channel_names);
    for_each_in_vector(ListCmdNames(*this, client), channel_names);
  }
  this->rpl_listend(client);
}
