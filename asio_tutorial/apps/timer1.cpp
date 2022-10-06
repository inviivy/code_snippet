#include <boost/asio.hpp>
#include <iostream>

int main() {
  boost::asio::io_context io;
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(10));
  t.wait();
  std::cout << "timer1: hello world\n";
}