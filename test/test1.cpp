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

#include "../include/event_framework.h"
// #include "../include/timer_manager.h"

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

class ScheduledEvent : public Event {
   public:
    Type GetType() const override { return GetEventType<ScheduledEvent>(); }
    std::string data;
    int scheduled_time;
    int count;
};
int main() {
    EventFramework framework;
    framework.Start();

    {
        framework.RegisterHandler<IntEvent>([](IntEvent& e) {
            std::cout << "处理MyEvent，数据：" << e.data << "Priority:" << e.GetPriority()
                      << std::endl;
        });

        auto event = std::make_unique<IntEvent>();
        event->data = 42;
        event->SetPriority(1);
        framework.PostEvent(std::move(event));
    }

    {
        framework.RegisterHandler<StrEvent>([](StrEvent& e) {
            std::cout << "处理StrEvent，数据：" << e.data << "Priority:" << e.GetPriority()
                      << std::endl;
        });

        auto event = std::make_unique<StrEvent>();
        event->data = "string dara 1";
        event->SetPriority(20);
        framework.PostEvent(std::move(event));
    }

    {
        framework.RegisterHandler<ScheduledEvent>([](ScheduledEvent& e) {
            std::cout << "处理ScheduledEvent，数据：" << e.data << "Priority:" << e.GetPriority()
                      << "count" << e.count << std::endl;
        });

        auto event = std::make_unique<ScheduledEvent>();
        event->data = "ScheduledEvent data 1";
        event->scheduled_time = 1;  // 1s后执行
        event->count = 0;
        event->SetPriority(20);
        framework.PostEvent(std::move(event));
    }

    // {
    //     // 在框架初始化时注册
    //     framework.RegisterHandler<TimerEvent>([](TimerEvent& e) {
    //         if (e.callback) e.callback();
    //     });
    //     // std::cout << "TimerEvent data 1" << std::endl;
    //     // 注册定时任务（每10秒执行）
    //     framework.AddTimer(
    //         std::chrono::seconds(1),
    //         []() {
    //             std::cout << "定时任务执行: "
    //                       << std::chrono::system_clock::now().time_since_epoch().count()
    //                       << std::endl;

    //         },
    //         10);
    // }

    {
        // 实现100ms累计时长
        framework.RegisterHandler<TimerEvent>([](TimerEvent& e) {
            if (e.callback) e.callback();
            // if(e.is_first){
            //     e.timestamp = std::chrono::steady_clock::now(); // 初始化 timestamp 为当前时间
            //     e.is_first = false;
            //         std::cout << "处理CountTimerEvent, is_first" << std::endl;

            // }
            // else {

            // 使用静态变量存储上一次函数调用的时间点
            // 获取当前时间点
            std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

            // 计算时间差 (duration)
            std::chrono::duration<double> timeDiff = currentTime - e.timestamp;

            // 将时间差转换为毫秒 (ms)
            auto timeDiffMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count();

            // 更新 e.timestamp 为 currentTime，为下次调用做准备
            e.timestamp = currentTime;

            // 首次调用时，previousTime 未被初始化，时间差可能不准确。
            // 为了避免首次调用返回很大的时间差（可能为负值），可以做个特殊处理。

            if (timeDiffMs < 0 || timeDiffMs > 1000) {
                timeDiffMs = 0;  // 或者返回一个特定的值表示首次调用
            }

            std::cout << "timeDiffMs" << timeDiffMs << std::endl;
            e.timer_count += timeDiffMs;
            // }
            std::cout << "Time difference: " << e.timer_count << " ms" << std::endl;
        });
        // std::cout << "TimerEvent data 1" << std::endl;
        // 注册定时任务（每10秒执行）
        framework.AddTimer(
            std::chrono::milliseconds(100),
            []() {
                // std::cout << "定时任务执行: "
                //           << std::chrono::system_clock::now().time_since_epoch().count()
                //           << std::endl;
            },
            1000);
    }
    std::this_thread::sleep_for(std::chrono::seconds(6));
    framework.Stop();
    return 0;
}