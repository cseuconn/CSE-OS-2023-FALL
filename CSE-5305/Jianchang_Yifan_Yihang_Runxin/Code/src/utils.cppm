module;

#include <chrono>
#include <mutex>
#include <shared_mutex>

export module utils;

export class TimeSliceManager {
public:
  static TimeSliceManager& getInstance() {
    static TimeSliceManager instance;
    return instance;
  }

  TimeSliceManager(const TimeSliceManager&) = delete;
  TimeSliceManager& operator=(const TimeSliceManager&) = delete;

  void setTimeSlice(const std::chrono::milliseconds slice) {
    std::unique_lock lock(mutex_);
    time_slice_ = slice;
  }

  std::chrono::milliseconds getTimeSlice() const {
    std::shared_lock lock(mutex_);
    return time_slice_;
  }

private:
  TimeSliceManager() = default;
  ~TimeSliceManager() = default;

  mutable std::shared_mutex mutex_;
  std::chrono::milliseconds time_slice_{std::chrono::milliseconds(1000)};
};