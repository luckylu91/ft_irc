#include "Client.hpp"
#include "Channel.hpp"
#include "utils.hpp"

void Client::remove_channel(Channel const & channel) {
  std::vector<const Channel *>::iterator channel_position = find_in_pointer_vector(channel, this->channels);
  if (channel_position != this->channels.end()) {
    this->channels.erase(channel_position);
  }
}

