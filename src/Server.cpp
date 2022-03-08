#include "Server.hpp"
#include "utils.hpp"

Channel & Server::add_client_to_channel(Client & client, std::string channel_name) {
  std::vector<Channel>::iterator channel_position = find_channel_by_name(channel_name, this->channels);
  if (channel_position == this->channels.end())
  {
    this->channels.push_back(Channel(channel_name, &client));
    return this->channels.back();
  }
  else
    return *channel_position;
}

void Server::remove_client_from_channel(Client & client, std::string channel_name) {
  std::vector<Channel>::iterator channel_position = find_channel_by_name(channel_name, this->channels);
  if (channel_position == this->channels.end())
    return ;

  Channel & channel = *channel_position;
  std::vector<Client const *>::iterator client_position = find_in_pointer_vector(client, channel.clients);
  if (client_position == channel.clients.end())
    return ;

  if (channel.size() == 1) {
    this->channels.erase(channel_position);
  }
  else {
    channel.remove_client(client);
  }
  client.remove_channel(channel);
}
