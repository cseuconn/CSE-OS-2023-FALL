module;

#include "spdlog/spdlog.h"
#include <expected>
#include <chrono>
#include <thread>
#include <optional>
#include <string>
#include <queue>
#include <unordered_map>
#include <asio.hpp>
#include <coroutine>
#include <deque>

export module scheduler;
import policy;
import utils;

export class Scheduler {
 public:
  explicit Scheduler(asio::io_context& io_context)
      : io_context_(io_context),
        exit_flag_(false),
        time_slice_(TimeSliceManager::getInstance().getTimeSlice()) {
    startProcessing();
  }

  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;
  Scheduler(Scheduler&&) = delete;
  Scheduler& operator=(Scheduler&&) = delete;

  ~Scheduler() { exit_flag_.store(true); }

  void registerTask(const int pid, std::unique_ptr<Policy> policy,
                    std::chrono::milliseconds deadline,
                    std::chrono::steady_clock::time_point start_time) {
    asio::post(io_context_,
               [this, pid, policy = std::move(policy), deadline, start_time]() mutable {
                 messages_.emplace(Message::Register{pid, std::move(policy), deadline, start_time});
                 spdlog::info("PID {} registered", pid);
               });
  }

  void deregisterTask(const int pid) {
    asio::post(io_context_, [this, pid]() {
      messages_.emplace(Message::Deregister{pid});
      spdlog::info("PID {} deregistered", pid);
    });
  }

 private:
  enum class TaskState { Initiated, Embarked, Demoted, Imminent };

  struct Message {
    struct Register {
      int pid{};
      std::unique_ptr<Policy> policy;
      std::chrono::milliseconds deadline{};
      std::chrono::steady_clock::time_point start_time;
    };
    struct Deregister {
      int pid;
    };
    std::variant<Register, Deregister> content;
  };

  struct TaskInfo {
    std::unique_ptr<Policy> policy;
    TaskState state{};
    std::chrono::milliseconds deadline{};
    std::chrono::steady_clock::time_point start_time;
  };

  asio::io_context& io_context_;
  std::atomic<bool> exit_flag_;
  std::queue<Message> messages_;
  std::unordered_map<int, TaskInfo> registered_tasks_;
  std::deque<int> fifoPool_;
  std::deque<int> cfsPool_;
  std::chrono::milliseconds time_slice_;

  void startProcessing() {
    asio::co_spawn(io_context_, [this] { return processMessages(); }, asio::detached);
    asio::co_spawn(io_context_, [this] { return processFIFOPool(); }, asio::detached);
    asio::co_spawn(io_context_, [this] { return processCFSPool(); }, asio::detached);
    asio::co_spawn(io_context_, [this] { return dispatcher(); }, asio::detached);
  }

  asio::awaitable<void> processMessages() {
    asio::steady_timer timer(io_context_);
    while (!exit_flag_.load()) {
      if (!messages_.empty()) {
        Message message = std::move(messages_.front());
        messages_.pop();

        std::visit(
            [this]<typename T0>(T0&& msg) {
              using T = std::decay_t<T0>;
              if constexpr (std::is_same_v<T, Message::Register>) {
                registered_tasks_[msg.pid] = TaskInfo{std::move(msg.policy), TaskState::Initiated,
                                                      msg.deadline, msg.start_time};
              } else if constexpr (std::is_same_v<T, Message::Deregister>) {
                registered_tasks_.erase(msg.pid);
              }
            },
            message.content);
      }
      co_await timer.async_wait(asio::use_awaitable);
    }
  }

  asio::awaitable<void> processFIFOPool() {
    asio::steady_timer timer(io_context_);
    while (!exit_flag_.load(std::memory_order_acquire)) {
      if (!fifoPool_.empty()) {
        int pid = fifoPool_.front();
        fifoPool_.pop_front();

        if (!registered_tasks_.contains(pid)) {
          co_await timer.async_wait(asio::use_awaitable);
          continue;
        }

        switch (const auto& task = registered_tasks_[pid]; task.state) {
          case TaskState::Embarked: {
            if (auto result = task.policy->apply(pid); result) {
              spdlog::info("PID {} switched to Embarked from Initiated", pid);
            } else {
              spdlog::error("PID {} failed to switch to Embarked: {}", pid, result.error());
            }
            break;
          }
          case TaskState::Imminent: {
            auto policy = std::make_unique<FIFO>(99, std::nullopt);
            if (auto result = policy->apply(pid); result) {
              spdlog::info("PID {} switched to Embarked with highest priority from Imminent", pid);
            } else {
              spdlog::error("PID {} failed to switch to Embarked with highest priority: {}", pid,
                            result.error());
            }
            break;
          }
          default: {
            spdlog::error("PID {} is not in Embarked or Imminent state", pid);
            break;
          }
        }
      }
      co_await timer.async_wait(asio::use_awaitable);
    }
  }

  asio::awaitable<void> processCFSPool() {
    asio::steady_timer timer(io_context_);
    while (!exit_flag_.load(std::memory_order_acquire)) {
      if (!cfsPool_.empty()) {
        int pid = cfsPool_.front();
        cfsPool_.pop_front();

        if (!registered_tasks_.contains(pid)) {
          co_await timer.async_wait(asio::use_awaitable);
          continue;
        }

        const auto& task = registered_tasks_[pid];
        if (const auto result = task.policy->apply(pid); result) {
          spdlog::info("PID {} switched to CFS from Embarked", pid);
        }
      }
      co_await timer.async_wait(asio::use_awaitable);
    }
  }

  asio::awaitable<void> dispatcher() {
    asio::steady_timer timer(io_context_);
    while (!exit_flag_.load(std::memory_order_acquire)) {
      time_slice_ = TimeSliceManager::getInstance().getTimeSlice();
      auto now = std::chrono::steady_clock::now();

      for (auto& task_entry : registered_tasks_) {
        switch (auto& [pid, task] = task_entry; task.state) {
          case TaskState::Initiated:
            task.state = TaskState::Embarked;
            fifoPool_.push_back(pid);
            break;
          case TaskState::Embarked:
            if (now - task.start_time >= time_slice_) {
              task.state = TaskState::Demoted;
              cfsPool_.push_back(pid);
            }
            break;
          case TaskState::Demoted:
            if (now - task.start_time > task.deadline - time_slice_) {
              task.state = TaskState::Imminent;
              fifoPool_.push_front(pid);
            }
            break;
          case TaskState::Imminent:
            // Logic for imminent state
            break;
          default:
            // Handle unknown state
            break;
        }
      }

      co_await timer.async_wait(asio::use_awaitable);
    }
  }
};