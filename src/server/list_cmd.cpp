#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils.hpp"
#include "numeric_codes.hpp"

/*
Command: LIST
Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]

The list command is used to list channels and their topics.  If the
<channel> parameter is used, only the status of that channel is
displayed.

If the <target> parameter is specified, the request is forwarded to
that server which will generate the reply.

Wildcards are allowed in the <target> parameter.

Numeric Replies:

       ERR_TOOMANYMATCHES              ERR_NOSUCHSERVER
       RPL_LIST                        RPL_LISTEND
*/

void Server::list_cmd(Client const * client, Message const & message) const {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channel_names;
	std::vector<Channel *> channels;

	if (args.size() == 0)
		for_each_in_vector(ListCmdPointers(*this, client), this->channels);
	else {
		split(args[0], ',', &channel_names);
		for_each_in_vector(ListCmdNames(*this, client), channel_names);
	}
	this->rpl_listend(client);
}
