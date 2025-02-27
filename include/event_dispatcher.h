#pragma once
#include <functional>
#include <memory>
#include <iostream>

class EventDispatcher {
   public:
    using Handler = std::function<void(Event&)>;

    void RegisterHandler(Event::Type type, Handler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_handlers[type].push_back(handler);
    }

    void Dispatch(Event& event) {
        std::vector<Handler> handlers;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_handlers.find(event.GetType());
            if (it != m_handlers.end())
            {handlers = it->second;
            }
            else {
                std::cout << "EventDispatcher::Dispatch: no handler for event type " << event.GetType() << std::endl;
            }
        }
        for (auto& handler : handlers) {
            handler(event);
        }
    }

   private:
    std::unordered_map<Event::Type, std::vector<Handler>> m_handlers;
    std::mutex m_mutex;
};