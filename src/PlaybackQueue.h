#pragma once
#include <queue>
#include <mutex>
#include <Arduino.h>

class PlaybackQueue {
public:
  void enqueue(const String& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(message);
  }

  bool hasNext() {
    std::lock_guard<std::mutex> lock(mutex_);
    return !queue_.empty();
  }

  String dequeue() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) return "";
    String front = queue_.front();
    queue_.pop();
    return front;
  }

private:
  std::queue<String> queue_;
  std::mutex mutex_;
};

// グローバルインスタンス
extern PlaybackQueue playbackQueue;
