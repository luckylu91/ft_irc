#pragma once

#include <cctype>
#include <string>
#include <vector>

#include "Client.hpp"
#include "SuffixTree.hpp"

class SuffixTree;

class Bot : public Client {
 public:
  Bot(Server& server, std::string const& name) : Client(server, name, "BOT") {}
  virtual ~Bot() {}
  virtual void receive_message(Message const& message) const;
  void parse_word_file(std::string const& file_name);

 private:
  SuffixTree tri;
};
