#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include "event.h"

class EventQueue {
public:
    void Push(std::unique_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(event));
        m_cond.notify_one();
    }

    std::unique_ptr<Event> Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] { return !m_queue.empty() || m_stopped; });
        if (m_stopped) return nullptr;
        auto event = std::move(m_queue.top());
        m_queue.pop();
        return event;
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopped = true;
        m_cond.notify_all();
    }

private:
    struct CompareEvent {
        bool operator()(const std::unique_ptr<Event>& a, const std::unique_ptr<Event>& b) const {
            return a->GetPriority() < b->GetPriority();
        }
    };
    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, CompareEvent> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_stopped = false;
};