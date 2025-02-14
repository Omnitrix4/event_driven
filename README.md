# event_driven

event.h 事件基类，用来定义事件类型和优先级（用户继承类创建业务）

event_queue.h 事件队列，用来存放事件push事件，pop事件

event_loop.h 独立线程运行的事件处理循环，驱动整个框架。

**EventDispatcher** ：事件处理器注册中心，注册和分发事件处理程序，

**EventFramework** ：用户直接操作的入口类，整合队列、分发器和事件循环。
