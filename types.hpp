#pragma once

#include <cstddef>
#include <string>

enum Command {
  KICK, // channel operator
  MODE, // channel operator
  INVITE, // channel operator (in +i mode)
  TOPIC, // channel operator (in +t mode)
  JOIN,
  NAMES,
  WHO,
  WHOIS,
};


enum ChannelMode {
  INVITE_ONLY, // +i
};
