class EventFramework {
public:
    EventFramework() : m_loop(m_queue, m_dispatcher) {}

    void Start() {
        m_thread = std::thread([this] { m_loop.Run(); });
    }

    void Stop() {
        m_queue.Stop();
        if (m_thread.joinable()) m_thread.join();
    }

    void PostEvent(std::unique_ptr<Event> event) {
        m_queue.Push(std::move(event));
    }

    template<typename T>
    void RegisterHandler(std::function<void(const T&)> handler) {
        auto type = GetEventType<T>();
        m_dispatcher.RegisterHandler(type, [handler](const Event& e) {
            const T* derived = dynamic_cast<const T*>(&e);
            if (derived) handler(*derived);
        });
    }

private:
    EventQueue m_queue;
    EventDispatcher m_dispatcher;
    EventLoop m_loop;
    std::thread m_thread;
};