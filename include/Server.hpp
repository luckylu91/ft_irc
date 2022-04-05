#pragma once

#include <exception>
#include <string>
#include <vector>

#include "Client.hpp"
#include "IOManager.hpp"

class Client;
class Message;
class Channel;
class Bot;
struct IOManagerInterface;

class Server {
 public:
  Server(std::string const &name, std::string const &version, std::string const &password, IOManagerInterface *io_manager);
  ~Server();

  // utils
  IOManagerInterface *get_io_manager() { return this->io_manager; }
  bool new_bot(std::string const &name, std::string const &file_name);
  void new_client(int sockfd, struct sockaddr_in addr);
  void remove_client(Client *client);
  void remove_client_sockfd(int sockfd);
  void remove_channel(Channel *channel);
  Client *find_client_by_sockfd(int sockfd) const;
  Client *find_client_by_nick(std::string const &nick) const;
  Client *find_clients_by_nickmask(std::string const &nickmask) const;
  Channel *find_channel_by_name(std::string const &name) const;
  bool try_password(std::string const &password) const;
  bool nick_exists(std::string const &nick) const;
  void receive_message(int sockfd, Message &message);

  // commands
  void msg_cmd(std::string const &command, Client const *source, std::string const &msgtarget,
               std::string const &message);
  void join_cmd(Client *client, Message const &message);
  void join_cmd_one(Client *client, std::string chan_name);
  void mode_cmd(Client *client, Message const &message);
  void invite_cmd(Client *client, Message const &message);
  Channel *try_action_on_channel_name(Client const *client, std::string const &channel_name);
  void kick_cmd(Client *src, Message const &message);
  void kick_one_cmd(Client *src, std::string const &channel_name, std::string const &dest_name,
                    std::string const &kick_message);
  void part_cmd(Client *client, Message const &message);
  void part_one_cmd(Client *client, std::string const &channel_name, std::string const &part_message);
  void list_cmd(Client const *client, Message const &message) const;

  // Numeric responses
  void send_message(Client const *, Message const &message) const;
  Message base_message(Client const *client, std::string const &command) const;
  Message base_message_no_nick(std::string const &command) const;

  // registation
  void rpl_welcome(Client const *client) const;
  void rpl_yourhost(Client const *client) const;
  void rpl_created(Client const *client) const;
  void rpl_myinfo(Client const *client) const;
  void welcome(Client const *client) const;

  // join
  void rpl_join(Client const *client, Channel const *chan) const;
  void rpl_namreply(Client const *client, Channel const *chan) const;
  void rpl_endofnames(Client const *client, Channel const *chan) const;
  int validity_test(Client *client, Channel *channel);
  // pong
  void rpl_pong(Client const *client) const;
  // invite
  void rpl_inviting(Client const *client, Channel const *channel, Client const *target) const;
  // kick & part
  void rpl_part_and_remove(Client *client, Channel *channel, std::string const &part_message) const;
  void rpl_kick_and_remove(Client const *src, Client *dest, Channel *channel, std::string const &kick_message) const;
  // quit
  void rpl_quit(Client const *client, std::string const &quit_msg = std::string());
  // topic
  void topic_cmd(Client *client, Message const &message);
  void rpl_notopic(Client const *client, Channel const *chan) const;
  void rpl_topic(Client const *client, Channel const *channel) const;
  void rpl_topic_set(Client const *client, Channel const *channel) const;
  // list
  void rpl_list(Client const *client, Channel const *channel) const;
  void rpl_listend(Client const *client) const;
  // mode
  void rpl_channelmodeis(Client const *client, Channel const *channel) const;
  // nick
  void rpl_nick(Client const *client, std::string const nick);
  // ban
  void rpl_banlist(Client const *client, Channel const *channel, std::string const nick);
  void rpl_endofbanlist(Client const *client, Channel const *channel);
  // errors
  void err_needmoreparams(Client const *client, std::string const &command) const;
  void err_alreadyregistred(Client const *client) const;
  void err_nonicknamegiven(Client const *client) const;
  void err_erroneusnickname(Client const *client, std::string const &nick) const;
  void err_nicknameinuse(Client const *client, std::string const &nick) const;  // attempt to change to a currently existing nickname.
  void err_restricted(Client const *client) const;
  void err_passwdmismatch(Client const *client) const;
  void err_alreadyregistered(Client const *client) const;
  void err_nosuchnick(Client const *client, std::string const &nick) const;
  void err_norecipient(Client const *client, std::string const &command) const;
  void err_notexttosend(Client const *client) const;
  void err_nosuchchannel(Client const *client, std::string const &channel_name) const;
  void err_unknownmode(Client const *client, std::string const &flag, std::string const &channel_name) const;
  void err_chanoprivsneeded(Client const *client, Channel const *channel) const;
  void err_inviteonlychan(Client const *client, Channel const *channel) const;
  void err_usernotinchannel(Client const *client, std::string const &nick, Channel const *channel) const;
  void err_notonchannel(Client const *client, Channel const *channel) const;
  void err_badchanmask(Client const *client, Channel const *channel) const;
  void err_useronchannel(Client const *client, Channel const *channel, Client const *target) const;
  void err_cannotsendtochan(Client const *client, std::string const &channel_name) const;
  void err_bannedfromchan(Client const *client, Channel const *channel) const;

 private:
  std::string name;
  std::string version;
  std::string password;
  std::vector<Client *> clients;
  std::vector<Channel *> channels;
  std::string creation_time_string;
  IOManagerInterface *io_manager;
};
