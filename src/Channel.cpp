#include "Channel.hpp"
#include "Client.hpp"
#include "utils.hpp"

void Channel::remove_client(Client const & client) {
  std::vector<const Client *>::iterator client_position = find_in_pointer_vector(client, this->clients);
  if (client_position != this->clients.end()) {
    this->clients.erase(client_position);
  }
}
