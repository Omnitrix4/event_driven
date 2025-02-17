#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <ctime>

enum class MessageType {
    HIGH_HEART_RATE,      // P0
    STORM_ALERT,          // P0
    INCOMING_CALL,        // P1
    ALARM,                // P2
    CHARGING_REMINDER,    // P3
    SUNRISE_SUNSET,       // P4
    DAILY_STEP_GOAL,      // P4
    DAILY_CALORIE_GOAL,   // P4
    WEEKLY_EXERCISE_GOAL, // P4
    WEEKLY_SUMMARY,       // P5
    PHONE_NOTIFICATION    // P5
};

struct Message {
    int id;
    time_t triggerTime;
    int priority;
    MessageType type;

    Message(time_t t, int p, MessageType mt) :
        triggerTime(t), priority(p), type(mt) {}
};

// 主队列比较函数（时间优先，其次优先级）
struct CompareMainQueue {
    bool operator()(const Message& a, const Message& b) {
        if(a.triggerTime == b.triggerTime) {
            return a.priority > b.priority; // 数值小的优先级高
        }
        return a.triggerTime > b.triggerTime; // 时间早的优先
    }
};

// 等待队列比较函数（优先级优先）
struct CompareWaitQueue {
    bool operator()(const Message& a, const Message& b) {
        return a.priority > b.priority; // 数值小的优先级高
    }
};

class MessageQueue {
private:
    int nextId = 1;
    bool isShowing = false;
    bool doNotDisturb = false;
    Message currentPopup = {-1, 0, 0, MessageType::HIGH_HEART_RATE};

    std::priority_queue<Message, std::vector<Message>, CompareMainQueue> mainQueue;
    std::priority_queue<Message, std::vector<Message>, CompareWaitQueue> waitQueue;
    std::vector<Message> messageCenter;
    std::unordered_set<int> deletedIds;

    bool shouldStore(MessageType type) {
        switch(type) {
            case MessageType::DAILY_STEP_GOAL:
            case MessageType::DAILY_CALORIE_GOAL:
            case MessageType::WEEKLY_EXERCISE_GOAL:
            case MessageType::WEEKLY_SUMMARY:
            case MessageType::PHONE_NOTIFICATION:
                return true;
            default:
                return false;
        }
    }

    bool isP5Timeout(const Message& msg, time_t current) {
        return msg.priority == 5 && (current - msg.triggerTime) > 60;
    }

    void showPopup(const Message& msg) {
        // 实际显示弹窗的实现
        std::cout << "Show popup: " << msg.id << std::endl;
    }

public:
    void addMessage(time_t triggerTime, int priority, MessageType type) {
        mainQueue.emplace(nextId++, triggerTime, priority, type);
    }

    void removeMessage(int id) {
        deletedIds.insert(id);
    }

    void setDoNotDisturb(bool enable) {
        doNotDisturb = enable;
    }

    void process(time_t currentTime) {
        // 处理主队列
        while(!mainQueue.empty()) {
            auto& msg = mainQueue.top();

            if(msg.triggerTime > currentTime) break;

            if(deletedIds.count(msg.id)) {
                mainQueue.pop();
                continue;
            }

            Message current = msg;
            mainQueue.pop();

            // 勿扰模式过滤
            if(doNotDisturb && (current.priority == 4 || current.priority == 5)) {
                if(shouldStore(current.type)) {
                    messageCenter.push_back(current);
                }
                continue;
            }

            // P5超时检查
            if(isP5Timeout(current, currentTime)) {
                messageCenter.push_back(current);
                continue;
            }

            // 处理消息显示
            if(isShowing) {
                waitQueue.push(current);
            } else {
                showPopup(current);
                isShowing = true;
                currentPopup = current;
            }
        }
    }

    void closePopup(time_t currentTime, bool userClose) {
        if(!isShowing) return;

        // 处理P5用户关闭的情况
        if(userClose && currentPopup.priority == 5) {
            if(currentPopup.type == MessageType::WEEKLY_SUMMARY ||
               currentPopup.type == MessageType::PHONE_NOTIFICATION) {
                messageCenter.push_back(currentPopup);
            }
        }

        isShowing = false;

        // 处理等待队列
        while(!waitQueue.empty()) {
            auto msg = waitQueue.top();
            waitQueue.pop();

            if(deletedIds.count(msg.id)) {
                deletedIds.erase(msg.id);
                continue;
            }

            // 重新检查超时和状态
            if(doNotDisturb && (msg.priority == 4 || msg.priority == 5)) {
                if(shouldStore(msg.type)) messageCenter.push_back(msg);
                continue;
            }

            if(isP5Timeout(msg, currentTime)) {
                messageCenter.push_back(msg);
                continue;
            }

            showPopup(msg);
            isShowing = true;
            currentPopup = msg;
            break;
        }
    }

    // 辅助方法
    const std::vector<Message>& getMessageCenter() const { return messageCenter; }
};

int main() {
    MessageQueue mq;

    // 示例使用
    time_t now = time(nullptr);
    mq.addMessage(now, 0, MessageType::HIGH_HEART_RATE);      // P0
    mq.addMessage(now, 5, MessageType::PHONE_NOTIFICATION);   // P5

    mq.process(now);
    // 第一个弹窗显示P0消息

    // 用户关闭弹窗
    mq.closePopup(now, false);
    // 处理等待队列中的P5消息

    return 0;
}