#pragma once
#include <string>
#include "Client.hpp"

class Channel
{
private:
  std::string name; // max len: 200; begins with '&'|'#'; no space, ^G or ','
  Client & chop; // channel operator, has leading '@'
};

