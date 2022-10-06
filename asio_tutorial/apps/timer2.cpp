#include <boost/asio.hpp>
#include <iostream>

void print(const boost::system::error_code &) {
  std::cout << "timer2: hello world\n";
}

int main() {
  boost::asio::io_context io;
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(10));
  t.async_wait(print);
  io.run();
  return 0;
}