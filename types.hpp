#pragma once

enum commands {
  KICK, // channel operator
  MODE, // channel operator
  INVITE, // channel operator (in +i mode)
  TOPIC, // channel operator (in +t mode)
  JOIN,
  NAMES,
  WHO,
  WHOIS,
};

enum modes {
  INVITE_ONLY, // +i
};
