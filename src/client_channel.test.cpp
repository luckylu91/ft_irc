#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <iostream>

int error_count = 0;

template <class Value>
void assert_value(std::string const & message, Value const & v1, Value const & v2) {
  std::cout << message << " (should be " << v1 << "): " << v2 << std::endl;
  if (v1 != v2) {
    error_count += 1;
  }
}

void test_client_in_channel() {
  Client client1("a$55p", "durion dusselier", "dudu55");
  Client client2("aasc.a3", "joseph joestar", "jojo");
  Channel channel1("quebec_libre", &client1);

  assert_value("client1 is in channel1", true, is_in_pointer_vector(client1, channel1.clients));
  assert_value("client2 is in channel1", false, is_in_pointer_vector(client2, channel1.clients));
}

void test_add_client_to_channel() {
  Client client1("a$55p", "durion dusselier", "dudu55");
  Client client2("aasc.a3", "joseph joestar", "jojo");
  Channel channel1("quebec_libre", &client1);

  add_client_to_channel(client1, channel1);
  assert_value("channel1 number of clients", (size_t)1, channel1.clients.size());
  add_client_to_channel(client2, channel1);
  assert_value("channel1 number of clients", (size_t)2, channel1.clients.size());
}

int main() {
  std::cout << std::boolalpha;

  test_client_in_channel();
  std::cout << "\n---*---\n" << std::endl;
  test_add_client_to_channel();

  std::cout << "\n---*---\n" << std::endl;
  std::cout << "The total error count is " << error_count << std::endl;
}
