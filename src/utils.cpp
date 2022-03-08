#include "utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"

struct ChannelNameEqChannel {
  bool operator()(std::string const & name, Channel const & channel) {
    return name == channel.name;
  }
};

std::vector<Channel>::iterator find_channel_by_name(std::string const & name, std::vector<Channel> & channels) {
  IsSame<std::string, Channel, ChannelNameEqChannel > compare_to_name(name);
  return std::find_if(
    channels.begin(),
    channels.end(),
    compare_to_name
  );
}

std::vector<Channel>::const_iterator find_channel_by_name(std::string const & name, std::vector<Channel> const & channels) {
  return static_cast< std::vector<Channel>::const_iterator >(
    find_channel_by_name(name, const_cast< std::vector<Channel> & >(channels))
  );
}

bool channel_name_in_list(std::string const & name, std::vector<Channel> const & channels) {
  return find_channel_by_name(name, channels) != channels.end();
}

void add_client_to_channel(Client & client, Channel & channel) {
  if (!is_in_pointer_vector(client, channel.clients)) {
    channel.clients.push_back(&client);
    client.channels.push_back(&channel);
  }
}
