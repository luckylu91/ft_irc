#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void	Server::mode_cmd(Client * client, Message const & message) {
	std::vector<std::string> temp_param = message.get_param();
	Channel * channel;
	if(temp_param[0][0]=='#')
	{
		try{
			channel = find_channel_by_name(temp_param[0]);
		if(!is_in_vector(client, channel->get_operators()))
		{
			err_chanoprivsneeded(client, channel);
			return;
		}
			channel->mode_cmd_channel(client,message);
		}
		catch (NoSuchChannelNameException &)
		{
			this->err_nosuchchannel(client, temp_param[0]);
		}
		catch (NoSuchClientNickException &)
		{
			this->err_nosuchnick(client, temp_param[2]);
		}
	}
}
