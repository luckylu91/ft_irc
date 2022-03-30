#pragma once

#include <string>
#include <fstream>
#include "IOManagerInterface.hpp"

class Server;

class MockIOManager: public IOManagerInterface {
private:
  std::ofstream & output_file;
public:
  MockIOManager(std::ofstream & output_file): output_file(output_file) {}
	void new_connection(Server * server, int client_sockfd);
  void read_string(Server * server, int client_sockfd, std::string const & s);
  virtual void send_message(int client_sockfd, Message const & message);
};
