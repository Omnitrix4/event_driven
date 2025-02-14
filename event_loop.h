
#pragma once
#include "event.h"
#include "event_queue.h"
#include "event_dispatcher.h"
#include "event_loop.h"

class EventLoop {
public:
    EventLoop(EventQueue& queue, EventDispatcher& dispatcher)
        : m_queue(queue), m_dispatcher(dispatcher) {}

    void Run() {
        while (true) {
            auto event = m_queue.Pop();
            if (!event) break;
            m_dispatcher.Dispatch(*event);
        }
    }

private:
    EventQueue& m_queue;
    EventDispatcher& m_dispatcher;
};