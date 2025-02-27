#pragma once
#include <thread>

#include "event.h"
#include "event_dispatcher.h"
#include "event_loop.h"
#include "event_queue.h"
#include "timer_manager.h"

class EventFramework {
   public:
    EventFramework() : m_loop(m_queue, m_dispatcher) {}

    void AddTimer(std::chrono::milliseconds interval,
                 std::function<void()> callback,
                 size_t repeat_count) {
        m_timerManager.AddTimer(interval, callback, repeat_count);
    }

    void Start() {
        m_thread = std::thread([this] { m_loop.Run(); });
        m_timerThread = std::thread([this] {
            m_timerManager.Run(*this);
        });
    }

    void Stop() {
        m_queue.Stop();
        m_timerManager.Stop();
        if (m_thread.joinable()) m_thread.join();
        if (m_timerThread.joinable()) m_timerThread.join();
    }
    void PostEvent(std::unique_ptr<Event> event) { m_queue.Push(std::move(event)); }

    template <typename T>
    void RegisterHandler(std::function<void( T&)> handler) {
        auto type = GetEventType<T>();
        m_dispatcher.RegisterHandler(type, [handler]( Event& e) {
             T* derived = dynamic_cast< T*>(&e);
            if (derived) handler(*derived);
        });
    }

   private:
    EventQueue m_queue;
    EventDispatcher m_dispatcher;
    EventLoop m_loop;
    std::thread m_thread;
    std::thread m_timerThread;
    TimerManager m_timerManager;
};