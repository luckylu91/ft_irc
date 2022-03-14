#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>
#include <iostream>

int Channel::add_client(Client * c)
{
    if( std::find(clients.begin(), clients.end(),c) == clients.end())
        {
            clients.push_back(c);
            c->add_chan(this);
            return 0;
        }
    else
    {
        std::cout<<"Debug message dans Add_client : Client deja dans le chan";
        return 1;
    }
}
std::string Channel::get_name()
{
    return name;
}
