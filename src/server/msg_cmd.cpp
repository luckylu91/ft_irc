#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

// PRIVMSG & NOTICE

static void send_msg_client_client(Server &server, Message &message, Client const *dest) {
  message.insert_param(0, dest->get_nick());
  server.send_message(dest, message);
}

static void send_msg_client_channel(Message &message, Client const *src, Channel const *channel) {
  message.insert_param(0, channel->get_name());
  channel->forward_message_except_sender(message, src);
}

void Server::msg_cmd(std::string const &command, Client const *src, std::string const &msgtarget, std::string const &message_str) {
  Message m;
  m.set_source(src->name());
  m.set_source_address(src);
  m.set_command(command);
  m.add_param(message_str);
  try {
    Client *dest_client = this->find_client_by_nick(msgtarget);
    send_msg_client_client(*this, m, dest_client);
  } catch (NoSuchClientNickException &) {
    try {
      Channel *dest_channel = this->find_channel_by_name(msgtarget);
      if (!dest_channel->contains_client(src))
        return this->err_cannotsendtochan(src, msgtarget);
      send_msg_client_channel(m, src, dest_channel);
    } catch (NoSuchChannelNameException &) {
      this->err_nosuchnick(src, msgtarget);
    }
  }
}
