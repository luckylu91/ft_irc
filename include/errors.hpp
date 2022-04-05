#pragma once

#include <cstdlib>
#include <exception>
#include <sstream>
#include <string>

class Client;

struct ClientSocketWriteException : std::exception {
  ClientSocketWriteException(Client const* client);
  virtual ~ClientSocketWriteException() throw();
  virtual const char* what() const throw();

  std::string msg;
};

struct NoSuchClientException : std::exception {
  std::string msg;
  void set_caracteristic(std::string const& client_caracteristic) {
    this->msg = "Client (" + client_caracteristic + ") not found";
  }
  virtual char const* what() const throw() { return this->msg.c_str(); }
  virtual ~NoSuchClientException() throw() {}
};

struct NoSuchClientSockFdException : NoSuchClientException {
  NoSuchClientSockFdException(int sockfd) : NoSuchClientException() {
    std::stringstream ss;
    ss << "sockfd = " << sockfd;
    this->set_caracteristic(ss.str());
  }
};

struct NoSuchClientNickException : NoSuchClientException {
  NoSuchClientNickException(std::string const& nick) {
    this->set_caracteristic("nick = " + nick);
  }
};

struct NoSuchChannelNameException : std::exception {
  std::string msg;
  NoSuchChannelNameException(std::string const& name) {
    this->msg = "Channel (name = " + name + ") not found";
  }
  virtual char const* what() const throw() { return this->msg.c_str(); }
  virtual ~NoSuchChannelNameException() throw() {}
};

struct EmptyVectorException : std::exception {
  virtual char const* what() const throw() { return "Empty vector"; }
};

struct FileReadError : std::exception {
  std::string msg;
  FileReadError(std::string const& file_name) : msg("Error reading file " + file_name) {}
  virtual ~FileReadError() throw() {}
  virtual char const* what() const throw() { return this->msg.c_str(); }
};
