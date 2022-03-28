#pragma once

#include <string>
#include <vector>

class Client;
class Channel;
class Message;
class Server;

struct SameNick {
  bool operator()(std::string const& nick, Client const* client);
};
struct SameSockfd {
  bool operator()(int sockfd, Client const* client);
};
struct SameChannelName {
  bool operator()(std::string const& name, Channel const* channel);
};
struct RemoveClientFromChannel {
  void operator()(Client* client, Channel* channel);
};
struct RemoveChannelFromClient {
  void operator()(Channel* channel, Client* client);
};

struct SendMessageToClient {
  void operator()(Message const& message, Client const* client);
};

struct SendMessageToClientDifferent {
  Client const* sender;
  Message const& message;
  SendMessageToClientDifferent(Client const* sender, Message const& message) : sender(sender), message(message) {}
  void operator()(Client const* client);
};

struct ListCmdNames {
  Server const& server;
  Client const* client;
  ListCmdNames(Server const& server, Client const* client) : server(server), client(client) {}
  void operator()(std::string const& channel_name);
};

struct ListCmdPointers {
  Server const& server;
  Client const* client;
  ListCmdPointers(Server const& server, Client const* client) : server(server), client(client) {}
  void operator()(Channel const* channel);
};

struct JoinCmd {
  Server& server;
  Client* client;
  JoinCmd(Server& server, Client* client) : server(server), client(client) {}
  void operator()(std::string const& channel_name);
};
