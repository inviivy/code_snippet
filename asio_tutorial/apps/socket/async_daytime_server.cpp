#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  time_t now = ::time(0);
  return ::ctime(&now);
}

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
  using pointer = std::shared_ptr<tcp_connection>;

  static pointer create(boost::asio::io_context &io_context) {
    return pointer(new tcp_connection(io_context));
  }

  tcp::socket &socket() { return m_socket; }

  void start() {
    m_message = make_daytime_string();
    boost::asio::async_write(
        m_socket, boost::asio::buffer(m_message),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

private:
  tcp_connection(boost::asio::io_context &io_context) : m_socket(io_context) {}
  void handle_write(const boost::system::error_code & /*error*/,
                    size_t /*bytes_transferred*/) {}

  tcp::socket m_socket;
  std::string m_message;
};

class tcp_server {
public:
  tcp_server(boost::asio::io_context &io_context)
      : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 9999)) {
    start_accept();
  }

private:
  void start_accept() {
    tcp_connection::pointer new_connection =
        tcp_connection::create(io_context_);

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&tcp_server::handle_accept, this,
                                       new_connection,
                                       boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection,
                     const boost::system::error_code &error) {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

  boost::asio::io_context &io_context_;
  tcp::acceptor acceptor_;
};

int main() {
  try {
    boost::asio::io_context io_context;
    tcp_server server(io_context);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}