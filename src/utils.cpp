#include <sstream>
#include <cstdint>
// #include <iomanip>      // std::setfill, std::setw
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
  ss << inet_ntoa(addr.sin_addr);
  return ss.str();
}

std::string client_name(std::string const & nick, std::string const & user_name, struct sockaddr_in addr) {
  std::stringstream ss;
  ss << nick;
  ss << "!" << user_name;
  ss << "@" << addr_string(addr);
  return ss.str();
}

bool address_equal(struct sockaddr_in addr1, struct sockaddr_in addr2) {
  return strcmp(inet_ntoa(addr1.sin_addr), inet_ntoa(addr1.sin_addr)) == 0
    && addr1.sin_port == addr2.sin_port;
}
