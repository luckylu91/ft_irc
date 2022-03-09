#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"
#include "NetworkClient.hpp"
#include "utils.hpp"
#include "numeric_codes.hpp"

struct Server {
  Server(std::string password): password(password), channels(), clients() {}

  Client * new_client(std::string nick, std::string real_name, std::string username);
  void new_channel(std::string name, Client * oper);
  Client * get_client_by_name(std::string const & nick) const;
  Client * get_client_by_addr(struct sockaddr_in addr) const;
  Client * get_channel_by_name(std::string const & name) const;
  bool is_correct_password(std::string const & password) const {
    return this->password == password;
  }

  std::string server_name();

  void reply_addr(struct sockaddr_in addr, std::string const & message);
  // :<source> <numeric_code> <client> <message>
  void reply_network_client(NetworkClient * client, char const * numeric_code, std::string const & message) {
    std::stringstream ss;
    ss << ":" << this->server_name();
    ss << " " << numeric_code;
    ss << " " << message;
    this->reply_addr(client-> ss.str()
  }

  void answer_welcome(struct sockaddr_in addr) {
    this->reply(addr, RPL_WELCOME,); // (001)
    this->reply(addr, RPL_YOURHOST); // (002)
    this->reply(addr, RPL_CREATED); // (003)
    this->reply(addr, RPL_MYINFO); // (004)
    this->reply(addr, RPL_ISUPPORT); // (005)
  }

  Channel & add_client_to_channel(Client & client, std::string channel_name);
  void remove_client_from_channel(Client & client, std::string channel_name);



  std::string password;
  std::vector<Channel *> channels;
  std::vector<Client *> clients;
  std::map<std::string, Client *> clients_id_table;
  std::map<struct sockaddr, NetworkClient *> clients_addr_table;
  std::map<std::string, Channel *> channel_id_table;

  std::string password;
};

