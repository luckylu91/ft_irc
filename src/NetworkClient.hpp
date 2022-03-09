#pragma once

#include <string>
#include <sstream>
#include "Server.hpp"
#include "numeric_codes.hpp"

struct NetworkClient {
  NetworkClient(Server & server, struct sockaddr_in addr):
    server(server),
    addr(addr),
    has_provided_password(false),
    nick_is_set(false),
    user_data_is_set(false) {}

  // ~NetworkClient() {
  //   this->server.remove_network_client();
  // }

  void set_password(std::string const & password) {
    if (this->is_registered()) {
      throw ERR_ALREADYREGISTERED;
    }
    if (this->server.is_correct_password(password)) {
      this->has_provided_password = true;
    } else {
      this->has_provided_password = false;
      throw ERR_PASSWDMISMATCH;
    }
  }

  void set_user_data(std::string const & user_name, std::string const & real_name) {
    if (this->is_registered()) {
      throw ERR_ALREADYREGISTERED;
    }
    this->user_name = user_name;
    this->real_name = real_name;
    this->user_data_is_set = true;
  }

  bool is_registered() const {
    return this->has_provided_password && this->nick_is_set;
  }

  void set_nick(std::string const & nick) {
    if (!this->has_provided_password) {
      throw //...
    }
    if (erroneus_nickname(nick)) {
      throw ERR_ERRONEUSNICKNAME;
    }
    if (this->server.has_nickname(nick)) {
      throw ERR_NICKNAMEINUSE;
    }
    this->nick = nick;
    this->nick_is_set = true;
    this->client = this->newly_registered_client();
  }

  bool erroneus_nickname(std::string const & nick) {

  }

  std::string client_name() {
    return this->is_registered() ? this->client->client_name() : ""
  }

  // Upon successful completion of the registration process, the server MUST send, in this order,
  // the RPL_WELCOME (001), RPL_YOURHOST (002), RPL_CREATED (003), RPL_MYINFO (004),
  // and at least one RPL_ISUPPORT (005) numeric to the client
  /* The server SHOULD then respond as though the client sent the LUSERS command and return the appropriate numerics.
     If the user has client modes set on them automatically upon joining the network,
     the server SHOULD send the client the RPL_UMODEIS (221) reply or a MODE message with the client as target,
     preferably the former. The server MAY send other numerics and messages.
     The server MUST then respond as though the client sent it the MOTD command, i.e. it must send either
     the successful Message of the Day numerics or the ERR_NOMOTD (422) numeric.
  */
  Client * newly_registered_client() {
    Client * client = this->server.new_client(this->nick, this->real_name, this->user_name);
    this->server.answer_welcome(this->addr);
    return client;
  }

  Server & server;
  struct sockaddr_in addr;
  bool has_provided_password;
  std::string nick; // len max: 9
  std::string user_name;
  std::string real_name;
  bool nick_is_set;
  bool user_data_is_set;
  Client * client;
};
