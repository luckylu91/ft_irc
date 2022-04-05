#include "utils.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <algorithm>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <errno.h>

std::string addr_string(struct sockaddr_in addr) {
  std::stringstream ss;
  ss << inet_ntoa(addr.sin_addr);
  return ss.str();
}

std::string special_string(std::string const& s) {
  std::size_t i;
  std::stringstream ss;

  for (i = 0; i < s.size(); i++) {
    if (!std::isspace(s[i]) || s[i] == ' ')
      ss << s[i];
    else {
      if (s[i] == '\f')
        ss << "\\f";
      else if (s[i] == '\n')
        ss << "\\n";
      else if (s[i] == '\r')
        ss << "\\r";
      else if (s[i] == '\t')
        ss << "\\t";
      else if (s[i] == '\v')
        ss << "\\v";
    }
  }
  return ss.str();
}

std::string split_return_remaining(std::string const& buffer, char sep, std::vector<std::string>* result_vec) {
  std::size_t start, stop;
  std::string token;

  start = 0;
  while (start < buffer.size()) {
    stop = buffer.find(sep, start);
    if (stop == std::string::npos) {
      return buffer.substr(start, std::string::npos);
    }
    if (stop > start) {
      token = buffer.substr(start, stop);
      result_vec->push_back(token);
    }
    start = stop + 1;
  }
  return std::string();
}

void split(std::string const& buffer, char sep, std::vector<std::string>* result_vec) {
  std::string remaining = split_return_remaining(buffer, sep, result_vec);
  if (remaining.size() > 0)
    result_vec->push_back(remaining);
}

void extract_lines(std::string const& file_name, std::vector<std::string>* lines) {
  std::fstream fs;
  std::string line;

  fs.open(file_name, std::fstream::in);
  if (fs.fail())
    throw FileReadError(file_name);
  while (std::getline(fs, line)) {
    if (line.empty())
      continue;
    lines->push_back(line);
  }
  fs.close();
}

void split_until(std::string& buffer, char delim, std::string* result_str) {
  std::size_t end_of_result = buffer.find(delim);
  *result_str = buffer.substr(0, end_of_result);
  std::size_t end_of_trim = buffer.find_first_not_of(delim, end_of_result);
  buffer.erase(0, end_of_trim);
}
