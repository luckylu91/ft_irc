#pragma once

#include <string>
#include <vector>

class Server;
class Client;
class Message;

class Channel {
 public:
  Channel(Server &server, std::string const &name, Client const *oper) :
    server(server),
    opers(1, oper),
    clients(),
    name(name),
    is_invite_only(0),
    is_private(0),
    is_secret(0),
    is_topic_protected(0) {}

  static bool invalid_channel_name(std::string const &name);
  void remove_client(Client *client);
  std::string get_name() const { return this->name; }
  bool get_is_invite_only() const { return this->is_invite_only; }
  bool get_is_secret() const { return this->is_secret; }
  bool get_is_private() const { return this->is_private; }
  bool get_is_topic_protected() const { return this->is_topic_protected; }
  std::string get_topic() const { return this->topic; };
  std::vector<Client const *> const &get_invited_vec() const { return this->invited; };
  std::vector<Client const *> const &get_operators() const { return this->opers; };
  std::vector<Client const *> const &get_banned_vec() const { return this->banned; };
  std::vector<Client const *> &get_clients() { return this->clients; }
  std::string op_cli_message() const;
  void forward_message(Message &message) const;
  void forward_message_except_sender(Message &message, Client const *sender) const;
  bool contains_client(Client const *client) const;
  void mode_cmd_channel(Client *client, Message const &message);
  void invite_cmd_channel(Client *client, std::vector<std::string> param);
  bool is_operator(Client const *client) const;
  bool is_normal_user(Client const *client) const;
  std::size_t size() const;
  void set_topic(std::string new_topic);
  std::string size_str() const;
  std::string mode_to_string() const;
  void add_client(Client const *client);

 private:
  Server &server;
  std::vector<Client const *> opers;
  std::vector<Client const *> clients;
  std::vector<Client const *> banned;
  std::vector<Client const *> invited;
  std::string name;
  std::string topic;
  bool is_invite_only;
  bool is_private;
  bool is_secret;
  bool is_topic_protected;
};
