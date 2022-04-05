#pragma once

class Message;

struct IOManagerInterface {
  virtual void send_message(int client_sockfd, Message const& message) = 0;
};
