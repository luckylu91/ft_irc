#include "Message.hpp"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.hpp"

typedef std::vector<std::string>::const_iterator string_iterator;
typedef std::map<int, std::string>::iterator int_string_iterator;

std::map<int, std::string> Message::sockfd_incomplete_recv;

void Message::set_source(std::string const &source) {
  this->source = source;
  this->source_is_set = true;
}
void Message::set_source_address(Client const *source) {
  this->source_address = source;
}
void Message::set_command(std::string const &command) {
  this->command = command;
}
void Message::add_param(std::string const &param) {
  this->param.push_back(param);
}

void Message::parse(std::string input, std::vector<Message> *vec, int sockfd) {
  std::size_t i;
  Message message;

  int_string_iterator it = Message::sockfd_incomplete_recv.find(sockfd);
  if (it != Message::sockfd_incomplete_recv.end()) {
    input = it->second + input;
    Message::sockfd_incomplete_recv.erase(it);
  }

  while (input.size() > 0) {
    i = input.find("\r\n", 0);
    if (i == std::string::npos) {
      Message::sockfd_incomplete_recv.insert(std::make_pair(sockfd, input));
      return;
    } else if (i > 0) {
      message = Message::parse_one(input.substr(0, i));
      vec->push_back(message);
    }
    input = input.substr(i + 2, std::string::npos);
  }
}

Message Message::parse_one(std::string s) {
  Message message;
  std::string tmp;
  if (s[0] == ':') {
    split_until(s, ' ', &tmp);
    message.set_source(tmp);
  }
  bool param_began = false;
  while (s.size() > 0 && s[0] != ':') {
    split_until(s, ' ', &tmp);
    if (!param_began) {
      message.set_command(tmp);
      param_began = true;
    } else
      message.add_param(tmp);
  }
  if (s.size() > 0) {
    std::size_t index_end = s.find("\r\n");
    message.add_param(s.substr(1, index_end));
  }
  return message;
}

void Message::remove_connection_cache(int sockfd) {
  int_string_iterator it = Message::sockfd_incomplete_recv.find(sockfd);
  if (it != Message::sockfd_incomplete_recv.end())
    Message::sockfd_incomplete_recv.erase(it);
}

static bool param_has_spaces(std::string const &str) {
  return str.find(' ') != std::string::npos;
}

static void insert_params_to_stream(std::stringstream &ss, std::vector<std::string> const &param) {
  if (param.size() == 0)
    return;
  string_iterator it_last = --param.end();
  for (string_iterator it = param.begin(); it != it_last; ++it) {
    ss << " " << *it;
  }
  ss << " ";
  if (param_has_spaces(*it_last))
    ss << ":";
  ss << *it_last;
}

std::string Message::to_string() const {
  std::stringstream ss;

  if (this->source_is_set)
    ss << ":" << this->source << " ";
  ss << this->command;
  insert_params_to_stream(ss, this->param);
  ss << "\r\n";
  return ss.str();
}

void Message::insert_param(std::size_t position, std::string const &param) {
  this->param.insert(this->param.begin() + position, param);
}
