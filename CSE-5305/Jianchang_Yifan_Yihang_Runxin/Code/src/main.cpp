#include "spdlog/spdlog.h"
#include <memory>
#include <expected>
#include <asio.hpp>

import scheduler;
import policy;
import utils;

using asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
 public:
  session(tcp::socket socket, Scheduler& scheduler)
      : socket_(std::move(socket)), scheduler_(scheduler), data_{} {}

  void start() { do_read(); }

 private:
  tcp::socket socket_;
  Scheduler& scheduler_;
  enum { max_length = 1024 };
  char data_[max_length];

  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, max_length),
                            [this, self](const std::error_code ec, const std::size_t length) {
                              if (!ec) {
                                const std::string message(data_, length);
                                process_message(message);
                                do_read();
                              }
                            });
  }

  void process_message(const std::string& message) {
    if (message == "getTimeSlice") {
      const auto time_slice = TimeSliceManager::getInstance().getTimeSlice();
      socket_.write_some(asio::buffer(std::to_string(time_slice.count())));
    } else if (const auto delimiter_pos = message.find(":"); delimiter_pos != std::string::npos) {
      const std::string command = message.substr(0, delimiter_pos);
      std::string value = message.substr(delimiter_pos + 1);

      if(command == "register") {
        const auto first_comma = value.find(",");
        const auto second_comma = value.find(",", first_comma + 1);
        const auto third_comma = value.find(",", second_comma + 1);

        if (first_comma != std::string::npos && second_comma != std::string::npos
            && third_comma != std::string::npos) {
          const int task_pid = std::stoi(value.substr(0, first_comma));
          int priority = std::stoi(value.substr(first_comma + 1, second_comma - first_comma - 1));
          int core = std::stoi(value.substr(second_comma + 1, third_comma - second_comma - 1));
          int deadline = std::stoi(value.substr(third_comma + 1));

          scheduler_.registerTask(task_pid, std::make_unique<FIFO>(priority, core),
                                  std::chrono::milliseconds(deadline),
                                  std::chrono::steady_clock::now());
        }
        socket_.write_some(asio::buffer("OK"));
      }
      else if (command == "deregister") {
        const int task_id = std::stoi(value);
        scheduler_.deregisterTask(task_id);
        socket_.write_some(asio::buffer("OK"));
      }
      else if (command == "setTimeSlice") {
        const int ms = std::stoi(value);
        TimeSliceManager::getInstance().setTimeSlice(std::chrono::milliseconds(ms));
        socket_.write_some(asio::buffer("OK"));
      }
      else { socket_.write_some(asio::buffer("Err")); }
    } else {
      socket_.write_some(asio::buffer("Err"));
    }
  }
};

class server {
 public:
  server(asio::io_context& io_context, const short port, Scheduler& scheduler)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), scheduler_(scheduler) {
    do_accept();
  }

 private:
  tcp::acceptor acceptor_;
  Scheduler& scheduler_;

  void do_accept() {
    acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
      if (!ec) { std::make_shared<session>(std::move(socket), scheduler_)->start(); }
      do_accept();
    });
  }
};

int main() {
  try {
    asio::io_context io_context;
    Scheduler scheduler(io_context);
    server srv(io_context, 1234, scheduler);
    io_context.run();
  } catch (std::exception& e) { spdlog::error("Exception: {}", e.what()); }

  return 0;
}