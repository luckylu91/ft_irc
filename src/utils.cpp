#include <sstream>
#include <cstdint>
#include <iomanip>      // std::setfill, std::setw
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


void add_client_to_channel(Client * client, Channel * channel) {
  if (!is_in(client, channel->clients)) {
    channel->clients.push_back(client);
    client->channels.push_back(channel);
  }
}

std::string addr_string(struct sockaddr_in addr) {
  std::stringstream ss;
  // ss << std::setfill('0') << std::setw(3);
  uint32_t ip_addr = static_cast<uint32_t>(addr.sin_addr);
  uint16_t ip_port = static_cast<uint16_t>(addr.sin_port);

  ss << (0xff000000 & ip_addr) << ".";
  ss << (0x00ff0000 & ip_addr) << ".";
  ss << (0x0000ff00 & ip_addr) << ".";
  ss << (0x000000ff & ip_addr) << ":";
  ss << ip_port;
}

std::string client_name(std::string const & nick, std::string const & user_name, struct sockaddr_in addr) {
  std::stringstream ss;
  ss << nick;
  ss << "!" << user_name;
  ss << "@" << addr_string(addr);
  return ss.str();
}
