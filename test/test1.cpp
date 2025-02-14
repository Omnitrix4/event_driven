#include "../event_framework.h"
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>


class MyEvent : public Event {
public:
    Type GetType() const override { return GetEventType<MyEvent>(); }
    int data;
};

int main() {
    EventFramework framework;
    framework.Start();

    framework.RegisterHandler<MyEvent>([](const MyEvent& e) {
        std::cout << "处理MyEvent，数据：" << e.data << std::endl;
    });

    auto event = std::make_unique<MyEvent>();
    event->data = 42;
    event->SetPriority(1);
    framework.PostEvent(std::move(event));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    framework.Stop();
    return 0;
}