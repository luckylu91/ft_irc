#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void	Server::parse_exe_join(Client * client, Message const & message)
{
	std::string temp = (message.get_param())[0];
	std::string chan_name;
	size_t find_value;
	while(!temp.empty())
	{
		// 		std::cout<<"debug dans parse "<<temp<<"\n";
		find_value = temp.find(',');
		if (find_value != std::string::npos)
		{
			chan_name = temp.substr(0,find_value);

			// 		std::cout<<"debug dans parse chan name1"<<chan_name<<"\n";
			temp.erase(0,find_value+1);
		}
		else
		{
			chan_name = temp;
			// 		std::cout<<"debug dans parse chan name2"<<chan_name<<"\n";
			temp.clear();
		}
		if (Channel::invalid_channel_name(chan_name)) {
			this->err_nosuchchannel(client, chan_name);
			continue ;
		}


		// 		std::cout<<"debug dans parse chan name3"<<chan_name<<"\n";
		join_cmd(client, chan_name);
	}
}
int Server::validity_test(Client * client, Channel * channel)
{
	if(is_in_vector(client, channel->get_banned_vec()))
	{
		err_bannedfromchan(client, channel);
		return 1;
	}
	if(channel->get_is_invite_only() && !is_in_vector(client, channel->get_invited_vec()))
	{
		err_inviteonlychan(client,channel);
		return 1;
	}
	return	0;
}
void Server::join_cmd(Client * client, std::string chan_name)
{
	Channel * channel;
	bool already_in_channel = false;
	try {
		channel = this->find_channel_by_name(chan_name);
		already_in_channel = channel->contains_client(client);
		std::cout<<"debug join_cmd is invite only = "<<channel->get_is_invite_only()<<"is_in_vector"<<is_in_vector(client, channel->get_invited_vec())<<std::endl;
		if (validity_test(client, channel))
			return;
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
