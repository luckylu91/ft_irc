#pragma once
#include <netinet/in.h>

#include <string>

class Message;
class Server;

void sig_handler(int sig_code);
void error(const char *msg);
int setup_socket(int &option, struct sockaddr_in &serv_addr, struct sockaddr_in &cli_addr, int &clilen, int portno);
void disconnection(struct kevent tevent, Server *server);
void check_main_arguments(int argc, char **argv);
