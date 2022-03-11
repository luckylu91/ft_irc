#include "errors.hpp"
#include "Client.hpp"

ClientSocketWriteException::ClientSocketWriteException(Client const * client):
  msg("Error Writing to socket" + client->name()) {}

const char * ClientSocketWriteException::what() const throw() {
  return msg.c_str();
}
