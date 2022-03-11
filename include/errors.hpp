#pragma once

#include <exception>
#include <string>

class Client;

struct ClientSocketWriteException: std::exception {
  ClientSocketWriteException(Client const * client);
  virtual ~ClientSocketWriteException() throw();
  virtual const char * what() const throw();

  std::string msg;
};
