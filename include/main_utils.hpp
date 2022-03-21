#pragma once
#include <string>

class Message;

void sig_handler(int sig_code);
void special_print(std::string const & s);
void error(const char *msg);
void debug_message_vec(std::vector<Message> & vec);

