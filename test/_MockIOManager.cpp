#include <vector>
#include <netinet/in.h>
#include "_MockIOManager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "utils.hpp"

void MockIOManager::read_string(Server * server, int client_sockfd, std::string const & s) {
  std::vector<Message> parsed_message;

  Message::parse(s, &parsed_message, client_sockfd);
  for (std::vector<Message>::iterator it = parsed_message.begin(); it != parsed_message.end(); it++)
    server->receive_message(client_sockfd, *it);
  this->output_file << client_sockfd << " -> '" << special_string(s) << "'";
}

void MockIOManager::new_connection(Server * server, int client_sockfd) {
  sockaddr_in client_addr;
  bzero(&client_addr, sizeof(sockaddr_in));
  server->new_client(client_sockfd, client_addr);
}

void MockIOManager::send_message(int client_sockfd, Message const & message) {
  this->output_file << "-> " << client_sockfd << " '" << special_string(message.to_string()) << "'";
}
