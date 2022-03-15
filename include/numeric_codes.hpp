#pragma once


#define RPL_WELCOME "001"
// "Welcome to the Internet Relay Network <nick>!<user>@<host>"
#define RPL_YOURHOST "002"
// "Your host is <servername>, running version <ver>"
#define RPL_CREATED "003"
// "This server was created <date>"
#define RPL_MYINFO "004"
// "<servername> <version> <available user modes> <available channel modes>"

#define RPL_NOTOPIC "331"
//reply: <channel> :No topic is set
#define RPL_NAMREPLY "353"
//reply: <channel> :[[@|+]<nick> [[@|+]<nick> [...]]]
#define ERR_NEEDMOREPARAMS "461"
// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED "462"
//  ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH "464"
// "<client> :Password incorrect"
#define ERR_NONICKNAMEGIVEN "431"
// ":No nickname given"
#define ERR_ERRONEUSNICKNAME "432"
// "<nick> :Erroneous nickname"
#define ERR_NICKNAMEINUSE "433"
// "<nick> :Nickname is already in use"
#define ERR_RESTRICTED "484"
// ":Your connection is restricted!"
#define ERR_NOSUCHNICK "401"
// "<nickname> :No such nick/channel"
