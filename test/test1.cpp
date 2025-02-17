#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "../event_framework.h"

class IntEvent : public Event {
   public:
    Type GetType() const override { return GetEventType<IntEvent>(); }
    int data;
};

class StrEvent : public Event {
   public:
    Type GetType() const override { return GetEventType<StrEvent>(); }
    std::string data;
};

int main() {
    EventFramework framework;
    framework.Start();

    {
        framework.RegisterHandler<IntEvent>([](const IntEvent& e) {
            std::cout << "处理MyEvent，数据：" << e.data << "Priority:" << e.GetPriority()
                      << std::endl;
        });

        auto event = std::make_unique<IntEvent>();
        event->data = 42;
        event->SetPriority(1);
        framework.PostEvent(std::move(event));
    }

    {
        framework.RegisterHandler<StrEvent>([](const StrEvent& e) {
            std::cout << "处理StrEvent，数据：" << e.data << "Priority:" << e.GetPriority()
                      << std::endl;
        });

        auto event = std::make_unique<StrEvent>();
        event->data = "string dara 1";
        event->SetPriority(20);
        framework.PostEvent(std::move(event));
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    framework.Stop();
    return 0;
}