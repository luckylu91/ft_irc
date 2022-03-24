#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils.hpp"

void Server::join_cmd(Client * client, Message const & message) {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channel_names;

	split(args[0], ',', &channel_names);
	for_each_in_vector(JoinCmd(*this, client), channel_names);
}


void Server::join_cmd_one(Client * client, std::string chan_name)
{
	Channel * channel;
	bool already_in_channel = false;
	try {
		channel = this->find_channel_by_name(chan_name);
		already_in_channel = channel->contains_client(client);
		std::cout<<"is invite only = "<<channel->get_is_invite_only()<<"is_in_vector"<<is_in_vector(client, channel->get_invited_vec())<<std::endl;
		if(channel->get_is_invite_only() && !is_in_vector(client, channel->get_invited_vec()))
		{
			err_inviteonlychan(client,chan_name);
			return;
		}
		add_if_no_in(client, channel->get_clients());
		add_if_no_in(channel, client->get_channels());
	}
	catch (NoSuchChannelNameException &) {
		channel = new Channel(*this, chan_name, client);
		channels.push_back(channel);
	}
	if (!already_in_channel) {
		this->rpl_join(client, channel);
		this->rpl_notopic(client, channel);
		this->rpl_namreply(client, channel);
		this->rpl_endofnames(client, channel);
	}
}
