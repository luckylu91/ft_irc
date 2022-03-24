#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

/*
   Channels names are strings (beginning with a '&' or '#' character) of
   length up to 200 characters.  Apart from the the requirement that the
   first character being either '&' or '#'; the only restriction on a
   channel name is that it may not contain any spaces (' '), a control G
   (^G or ASCII 7), or a comma (',' which is used as a list item
   separator by the protocol).
   */

bool Channel::invalid_channel_name(std::string const & name) {
	char illegal_chars[] = {
		' ', 7, ',', '\0'
	};
	if (name.find_first_of(illegal_chars) != std::string::npos)
		return true;
	if (name[0] != '#' && name[0] != '&')
		return true;
	if (name.size() > 200)
		return true;
	return false;
}

typedef std::vector<Client const *>::const_iterator client_iterator;
void Channel::remove_client(Client * client) {
	remove_from_vector(client, this->clients);
	remove_from_vector(client, this->opers);
	if (this->size() == 0)
		this->server.remove_channel(this);
}

std::string Channel::op_cli_message() const {
	std::string r;

	for(client_iterator it = clients.begin(); it != clients.end();it++)
	{
		if (it != clients.begin())
			r.append(" ");
		r.append((*it)->get_nick());
	}
	if (clients.size() > 0 && opers.size() > 0)
		r.append(" ");
	for(client_iterator it = opers.begin(); it != opers.end();it++)
	{
		if (it != opers.begin())
			r.append(" ");
		r.append("@");
		r.append((*it)->get_nick());
	}
	return r;
}

// void Channel::forward_message(Client const * src, std::string const & content) const {
// 	Message message;
// 	message.set_source(src->name());
// 	message.add_param(this->get_name());
// 	message.add_param(content);
// 	this->forward_message(message);
// }

void Channel::forward_message(Message & message) const {
	for_each_in_vector<SendMessageToClient>(message, this->clients);
	for_each_in_vector<SendMessageToClient>(message, this->opers);
}

void Channel::forward_message_except_sender(Message & message, Client const * sender) const {
	SendMessageToClientDifferent action(sender, message);
	for_each_in_vector(action, this->clients);
	for_each_in_vector(action, this->opers);
}

bool Channel::contains_client(Client const * client) const {
	return is_in_vector(client, this->opers) || is_in_vector(client, this->clients);
}

void	Channel::invite_cmd_channel(Client * client, std::vector<std::string> param)
{
	std::cout<<"DEBG Invte cnd channel 1\n";
	Client * target = server.find_client_by_nick(param[0]);
	std::cout<<"DEBG Invte cnd channel 2\n";
	if (!is_invite_only)
		return;
	std::cout<<"DEBG Invte cnd channel 3\n";
	if (!contains_client(client))
		return server.err_notonchannel(client,this);
	std::cout<<"DEBG Invte cnd channel 4\n";
	if (contains_client(target))
		return server.err_useronchannel(client,this,target);
	std::cout<<"DEBG Invte cnd channel 5\n";
	if (!is_operator(client))
		return server.err_chanoprivsneeded(client, this);
	std::cout<<"DEBG Invte cnd channel 6\n";
	invited.push_back(target);
	server.rpl_inviting(client,this,target);
}

void	Channel::mode_cmd_channel(Client * client, Message const & message)
{
	std::vector<std::string> temp_param = message.get_param();
	bool signe;
	Client * temp_client;
	if (temp_param[1][0] == '+')
		signe = 1;
	else if (temp_param[1][0] == '+')
		signe = 0;
	else
		return;
	for(std::string::iterator it = temp_param[1].begin() + 1; it !=temp_param[1].end(); it++)
	{
		if (*it =='b' ||*it == 'o')
		{
			temp_client = server.find_client_by_nick(temp_param[2]);
			if(*it == 'b')
			{
				if(signe)
				{
					banned.push_back(temp_client);
					//kicktodo
				}
				else
					remove_from_vector(temp_client , banned);
			}
			else
			{
				if(signe)
					opers.push_back(temp_client);
				else
					remove_from_vector(temp_client , opers);
			}
		}
		else if (*it == 'i')
		{
			if(signe && !is_invite_only)
				invited = clients;
			is_invite_only = signe;
		}
		else if (*it == 'p')
			is_private = signe;
		else if (*it == 's')
			is_secret = signe;
		else if (*it == 't')
			is_topic_protected = signe;
		else
		{
			server.err_unknownmode(client,std::string(1,*it), name);
			return;
		}
	}
}

bool Channel::is_operator(Client const * client) const {
	return is_in_vector(client, this->opers);
}

bool Channel::is_normal_user(Client const * client) const {
	return is_in_vector(client, this->clients);
}

std::size_t Channel::size() const {
	return this->clients.size() + this->opers.size();
}

std::string Channel::size_str() const {
	std::stringstream ss;
	ss << this->size();
	return ss.str();
}
