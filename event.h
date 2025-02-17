#pragma once
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Event {
   public:
    virtual ~Event() = default;
    using Type = size_t;
    virtual Type GetType() const = 0;
    int GetPriority() const { return m_priority; }
    void SetPriority(int priority) { m_priority = priority; }

   private:
    int m_priority = 0;
};

template <typename T>
size_t GetEventType() {
    static const size_t type = typeid(T).hash_code();
    return type;
}