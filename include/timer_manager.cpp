#include "timer_manager.h"
#include "event_framework.h"
#include <iostream>
#include <chrono>


 std::chrono::steady_clock::time_point  TimerEvent::timestamp = std::chrono::steady_clock::now();
 uint64_t TimerEvent::timer_count = 0;

void TimerManager::Run(EventFramework& framework) {
        while (m_running) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_tasks.empty()) {
                m_cond.wait(lock);
                continue;
            }

            auto task = m_tasks.top();
            auto now = std::chrono::steady_clock::now();

        // 将 steady_clock::time_point 转换为 system_clock::time_point
        auto now_system = std::chrono::system_clock::now() + (now - std::chrono::steady_clock::now());
        auto next_run_system = std::chrono::system_clock::now() + (task.next_run - std::chrono::steady_clock::now());

        // 将 system_clock::time_point 转换为 time_t
        auto now_time_t = std::chrono::system_clock::to_time_t(now_system);
        auto next_run_time_t = std::chrono::system_clock::to_time_t(next_run_system);

    // std::cout << "now: " << std::ctime(&now_time_t) << "task.next_run " << std::ctime(&next_run_time_t) << std::endl;

            if (now >= task.next_run) {
                m_tasks.pop();
                // std::cout << "TimerManager::Run:2 " << std::endl;
                // 推送定时事件到主队列
                framework.PostEvent(std::make_unique<TimerEvent>(task.callback));

                // 重复任务重新入队
                if (task.repeat_count > 0) {
                    task.next_run += task.interval;
                    task.repeat_count -= 1;
                                            std::cout << "定时任务执行剩余次数: "
                          << task.repeat_count
                          << std::endl;
                    m_tasks.push(task);
                }
            } else {
                m_cond.wait_until(lock, task.next_run);
            }
        }
    }