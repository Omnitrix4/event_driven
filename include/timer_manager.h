#pragma once
#include <chrono>
#include <condition_variable>

#include "event.h"

class EventFramework;

class TimerEvent : public Event {
   public:
    using Callback = std::function<void()>;

    TimerEvent(Callback cb) : callback(std::move(cb)) {
        // timestamp = std::chrono::steady_clock::now(); // 初始化 timestamp 为当前时间
    }
    Type GetType() const override { return GetEventType<TimerEvent>(); }
    static uint64_t timer_count;
    static std::chrono::steady_clock::time_point timestamp;
    bool is_first = true;
    Callback callback;
};

// class CountTimerEvent : public CountTimerEvent {
//    public:
//     Type GetType() const override { return GetEventType<CountTimerEvent>(); }
//     int timer_count;
//     int timestamp;
// };

// todo 1.实现移除定时任务，2，实现定时任务的暂停和恢复
//  3,实现间隔调整 4.实现事件优先级的设置
class TimerManager {
   public:
    void AddTimer(std::chrono::milliseconds interval, std::function<void()> callback,
                  size_t repeat_count) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push({std::chrono::steady_clock::now() + interval, interval, std::move(callback),
                      repeat_count});
        m_cond.notify_one();
    }

    void Run(EventFramework& framework);

    void Stop() {
        m_running = false;
        m_cond.notify_all();
    }

   private:
    struct TimerTask {
        std::chrono::steady_clock::time_point next_run;
        std::chrono::milliseconds interval;
        std::function<void()> callback;
        size_t repeat_count;

        bool operator<(const TimerTask& other) const {
            return next_run > other.next_run;  // 最小堆实现
        }
    };

    std::priority_queue<TimerTask> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_running = true;
};