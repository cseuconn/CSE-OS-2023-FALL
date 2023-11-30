module;

#include <sched.h>
#include <expected>
#include <string>
#include <optional>
#include <cerrno>
#include <print>

export module policy;

export enum class Result { OK, Err };

export class Policy {
 public:
  virtual std::expected<Result, std::string> apply(int pid) const = 0;
  virtual ~Policy() = default;
};

export class FIFO final : public Policy {
 public:
  explicit FIFO(const int priority, const std::optional<int> core = std::nullopt)
      : priority_(priority), core_(core) {}

  std::expected<Result, std::string> apply(const int pid) const override {
    sched_param param{};
    param.sched_priority = priority_;

    if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1) {
      // return std::unexpected("Error setting FIFO scheduler: " + std::to_string(errno));
      switch (errno) {
        case EPERM:
          return std::unexpected("The calling process does not have appropriate privileges.");
        case EINVAL: return std::unexpected("Invalid policy or param.");
        case ESRCH: return std::unexpected("The process whose ID is pid could not be found.");
        default: return std::unexpected("Unknown error.");
      }
    }

    if (core_.has_value()) {
      cpu_set_t set;
      CPU_ZERO(&set);
      CPU_SET(core_.value(), &set);
      if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
        // return std::unexpected("Error setting CPU affinity: " + std::to_string(errno));
        switch (errno) {
          case EFAULT:
            return std::unexpected("The set points outside the address space of the process.");
          case EINVAL: return std::unexpected("The set contains an invalid CPU number.");
          case EPERM:
            return std::unexpected("The calling process does not have appropriate privileges.");
          case ESRCH: return std::unexpected("The process whose ID is pid could not be found.");
          default: return std::unexpected("Unknown error.");
        }
      }
    }

    return Result::OK;
  }

 private:
  int priority_{};
  std::optional<int> core_{};
};

export class CFS final : public Policy {
 public:
  explicit CFS(const std::optional<int> core = std::nullopt) : core_(core) {}

  std::expected<Result, std::string> apply(const int pid) const override {
    sched_param param{};
    param.sched_priority = 0;

    if (sched_setscheduler(pid, SCHED_OTHER, &param) == -1) {
      // return std::unexpected("Error setting CFS scheduler: " + std::to_string(errno));
      switch (errno) {
        case EPERM:
          return std::unexpected("The calling process does not have appropriate privileges.");
        case EINVAL: return std::unexpected("Invalid policy or param.");
        case ESRCH: return std::unexpected("The process whose ID is pid could not be found.");
        default: return std::unexpected("Unknown error.");
      }
    }

    if (core_.has_value()) {
      cpu_set_t set;
      CPU_ZERO(&set);
      CPU_SET(core_.value(), &set);
      if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
        // return std::unexpected("Error setting CPU affinity: " + std::to_string(errno));
        switch (errno) {
          case EFAULT:
            return std::unexpected("The set points outside the address space of the process.");
          case EINVAL: return std::unexpected("The set contains an invalid CPU number.");
          case EPERM:
            return std::unexpected("The calling process does not have appropriate privileges.");
          case ESRCH: return std::unexpected("The process whose ID is pid could not be found.");
          default: return std::unexpected("Unknown error.");
        }
      }
    }

    return Result::OK;
  }

 private:
  std::optional<int> core_{};
};
