#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include <algorithm>


void Channel::remove_client(Client * client) {
  remove_from_vector(client, this->clients);
  remove_from_vector(client, this->opers);
}
