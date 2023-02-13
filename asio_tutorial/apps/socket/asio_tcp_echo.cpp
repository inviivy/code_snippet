#include <boost/asio.hpp>

#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

template <typename T>
struct slot : public std::enable_shared_from_this<slot<T>> {
  explicit slot(const std::shared_ptr<T> &obj_,
                std::function<void(const std::shared_ptr<T> &)> func)
      : weak_(obj_), func_(std::move(func)) {}

  ~slot() {
    if (func_) [[likely]] {
      auto obj = weak_.lock();
      if (obj) {
        func_(obj);
      }
    }
  }

private:
  std::weak_ptr<T> weak_;
  std::function<void(const std::shared_ptr<T> &)> func_;
};

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_.data(), 1024),
        [this, self = std::move(self)](boost::system::error_code ec,
                                       std::size_t length) {
          if (!ec) {
            do_write(length);
          } else {
            close();
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(data_.data(), length),
        [this, self = std::move(self)](boost::system::error_code ec,
                                       std::size_t /*length*/) {
          if (!ec) {
            do_read();
          }
        });
  }

  void close() {
    if (has_close_.test()) {
      return;
    }
    boost::asio::post(socket_.get_executor(),
                      [this, self = shared_from_this()] {
                        socket_.shutdown(tcp::socket::shutdown_both);
                        socket_.close();
                      });
    has_close_.test_and_set();
  }

private:
  tcp::socket socket_;
  std::array<char, 1024> data_;
  std::atomic_flag has_close_ = false;
};

class server {
public:
  server(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        signals_(io_context, SIGINT, SIGTERM) {
    signals_.async_wait(
        [this](const boost::system::error_code &error, int signal_number) {
          handle_signal(error, signal_number);
        });
    do_accept();
  }

  void close() {
    boost::asio::post(acceptor_.get_executor(), [this]() {
      boost::system::error_code ec;
      acceptor_.cancel(ec);
      if (ec) {
        // ...
      }

      acceptor_.close(ec);
      if (ec) {
        std::cout << ec.message() << '\n';
      }
    });
  }

private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!acceptor_.is_open()) {
            return;
          }
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }
          do_accept();
        });
  }

  void handle_signal(const boost::system::error_code &error,
                     int signal_number) {
    if (!error) {
      // a signal occurred
      std::cout << "a signal occurred, signal_number = " << signal_number
                << '\n';
      close();
    }
  }

private:
  tcp::acceptor acceptor_;
  boost::asio::signal_set signals_; // capture signals
};

int main() {
  boost::asio::io_context io_context;
  server s(io_context, 9999);
  io_context.run();
  return 0;
}