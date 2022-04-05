#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class Client;

class Message {
 public:
  Message() : source_is_set(false), source_address(NULL) {}
  void set_source(std::string const &source);
  void set_source_address(Client const *source);
  void set_command(std::string const &command);
  std::string get_source() const { return this->source; }
  Client const *get_source_address() const { return this->source_address; }
  std::string get_command() const { return this->command; }
  std::vector<std::string> const &get_param() const { return this->param; }
  void add_param(std::string const &param);
  void insert_param(std::size_t position, std::string const &param);
  std::string to_string() const;
  static void parse(std::string input, std::vector<Message> *vec, int sockfd);
  static Message parse_one(std::string s);
  static void remove_connection_cache(int sockfd);

 private:
  bool source_is_set;
  std::string source;
  std::string command;
  std::vector<std::string> param;
  Client const *source_address;

  static std::map<int, std::string> sockfd_incomplete_recv;
};
