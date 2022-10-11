#include <boost/asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  time_t now = ::time(0);
  return ctime(&now);
}

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 9999));
    for (;;) {
      // socket析构时会自动调用close
      tcp::socket socket(io_context);
      acceptor.accept(socket);
      std::string message = make_daytime_string();
      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message, message.size()),
                         ignored_error);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}