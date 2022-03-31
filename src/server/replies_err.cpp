#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "errors.hpp"
#include "numeric_codes.hpp"
#include "utils_template.tpp"

// ERR

#define ERR_BANNEDFROMCHAN "474"
// "<channel> :Cannot join channel (+b)"
//
void Server::err_bannedfromchan(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, ERR_BANNEDFROMCHAN);
  m.add_param(channel->get_name());
  m.add_param("Cannot join channel (+b)");
  this->send_message(client, m);
}
void Server::err_needmoreparams(Client const* client, std::string const& command) const {
  Message m = this->base_message(client, ERR_NEEDMOREPARAMS);
  m.add_param(command);
  m.add_param("Not enough parameters");
  this->send_message(client, m);
}
void Server::err_alreadyregistred(Client const* client) const {
  Message m = this->base_message(client, ERR_ALREADYREGISTRED);
  m.add_param("Unauthorized command (already registered)");
  this->send_message(client, m);
}
void Server::err_nonicknamegiven(Client const* client) const {
  Message m = this->base_message(client, ERR_NONICKNAMEGIVEN);
  m.add_param("No nickname given");
  this->send_message(client, m);
}
void Server::err_erroneusnickname(Client const* client, std::string const& nick) const {
  Message m = this->base_message(client, ERR_ERRONEUSNICKNAME);
  m.add_param(nick);
  m.add_param("Erroneous nickname");
  this->send_message(client, m);
}
void Server::err_nicknameinuse(Client const* client, std::string const& nick) const {
  Message m = this->base_message(client, ERR_NICKNAMEINUSE);
  m.add_param(nick);
  m.add_param("Nickname is already in use");
  this->send_message(client, m);
}
void Server::err_restricted(Client const* client) const {
  Message m = this->base_message(client, ERR_RESTRICTED);
  m.add_param("Your connection is restricted!");
  this->send_message(client, m);
}
void Server::err_passwdmismatch(Client const* client) const {
  Message m = this->base_message(client, ERR_PASSWDMISMATCH);
  m.add_param("Password incorrect");
  this->send_message(client, m);
}
void Server::err_nosuchnick(Client const* client, std::string const& nick) const {
  Message m = this->base_message(client, ERR_NOSUCHNICK);
  m.add_param(nick);
  m.add_param("No such nick/channel");
  this->send_message(client, m);
}
void Server::err_norecipient(Client const* client, std::string const& command) const {
  Message m = this->base_message(client, ERR_NORECIPIENT);
  m.add_param("No recipient given (" + command + ")");
  this->send_message(client, m);
}
void Server::err_chanoprivsneeded(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, ERR_CHANOPRIVSNEEDED);
  m.add_param(channel->get_name());
  m.add_param("You're not channel operator");
  this->send_message(client, m);
}
void Server::err_notexttosend(Client const* client) const {
  Message m = this->base_message(client, ERR_NOTEXTTOSEND);
  m.add_param("No text to send");
  this->send_message(client, m);
}
void Server::err_nosuchchannel(Client const* client, std::string const& channel_name) const {
  Message m = this->base_message(client, ERR_NOSUCHCHANNEL);
  m.add_param(channel_name);
  m.add_param("No such channel");
  this->send_message(client, m);
}
void Server::err_inviteonlychan(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, ERR_INVITEONLYCHAN);
  m.add_param(channel->get_name());
  m.add_param("Cannot join channel (+i)");
  this->send_message(client, m);
}
void Server::err_unknownmode(Client const* client, std::string const& flag, std::string const& channel_name) const {
  Message m = this->base_message(client, ERR_UNKNOWNMODE);
  m.add_param(flag);
  m.add_param("is unknown mode char to me for");
  m.add_param(channel_name);
}
void Server::err_usernotinchannel(Client const* client, std::string const& nick, Channel const* channel) const {
  Message m = this->base_message(client, ERR_USERNOTINCHANNEL);
  m.add_param(nick);
  m.add_param(channel->get_name());
  m.add_param("They aren't on that channel");
  this->send_message(client, m);
}
void Server::err_notonchannel(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, ERR_NOTONCHANNEL);
  m.add_param(channel->get_name());
  m.add_param("You're not on that channel");
  this->send_message(client, m);
}
void Server::err_badchanmask(Client const* client, Channel const* channel) const {
  Message m = this->base_message(client, ERR_BADCHANMASK);
  m.add_param(channel->get_name());
  m.add_param("Bad Channel Mask");
  this->send_message(client, m);
}
void Server::err_useronchannel(Client const* client, Channel const* channel, Client const* target) const {
  Message m = this->base_message(client, ERR_USERONCHANNEL);
  m.add_param(target->get_nick());
  m.add_param(channel->get_name());
  m.add_param("is already on channel");
}
void Server::err_cannotsendtochan(Client const* client, std::string const& channel_name) const {
  Message m = this->base_message(client, ERR_CANNOTSENDTOCHAN);
  m.add_param(channel_name);
  m.add_param("Cannot send to channel");
  this->send_message(client, m);
}
