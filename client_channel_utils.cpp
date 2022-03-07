#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

typedef std::vector<Client const *>::const_iterator  client_iterator;
typedef std::vector<Channel const *>::const_iterator channel_iterator;

template <class T>
struct IsSameAs {
  T const & a;

  IsSameAs(T const & a): a(a) {}
  bool operator()(T const * b) {
    return this->a == *b;
  }
};

client_iterator find_client_in_channel(Client const & client, Channel const & channel) {
  IsSameAs<Client> client_compare(client);
  return std::find_if(
    channel.clients.begin(),
    channel.clients.end(),
    client_compare
  );
}

void add_client_to_channel(Client & client, Channel & channel) {
  client_iterator search_result = find_client_in_channel(client, channel);
  if (search_result != channel.clients.end()) {
    // already there
    return ;
  }
  else {
    channel.clients.push_back(&client);
    client.channels.push_back(&channel);
  }
}
