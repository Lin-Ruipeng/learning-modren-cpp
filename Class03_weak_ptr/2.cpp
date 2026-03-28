#include <iostream>
#include <memory>

// 定义 IMU 节点
struct ImuNode
{
    int id;
    // TODO: 这里应该填什么？shared_ptr 还是 weak_ptr？
    // 场景：下一个节点引用上一个节点。如果是双向链表，这里容易造成循环引用。
    // 假设这是指向前一个节点的指针 prev
    std::weak_ptr<ImuNode> prev;   // 上节点用弱引用
    std::shared_ptr<ImuNode> next; // 下一节点用共享
    // 指向前一个链表的指针应该为弱引用!指向下一个节点的则应该用shared!
    // 这是为了避免环形引用导致的内存泄漏(计数不能为0 无法释放内存)

    ImuNode(int i) : id(i) { std::cout << "Node " << id << " created.\n"; }
    ~ImuNode() { std::cout << "Node " << id << " destroyed.\n"; }
};

// 任务一：打破循环引用
// 要求：修改上面的 struct ImuNode 定义中的 prev 指针类型，使其避免内存泄漏。
// 注意：这只需要你修改一行代码，并在提交时说明原因。

// 任务二：安全观测者
// 模拟监控线程函数
void monitorData(std::weak_ptr<int> dataWatcher)
{
    // TODO: 实现
    // 1. 使用 lock() 尝试获取数据
    // 2. 如果获取成功，打印 "Monitor: Data is " << *data
    // 3. 如果获取失败（对象已销毁），打印 "Monitor: Data expired!"
    if (auto data = dataWatcher.lock()) // .lock()尝试获取所有权
    {
        std::cout << "Monitor: Data is " << *data << std::endl;
    }
    else
    {
        std::cout << "Monitor: Data expired!" << std::endl;
    }
}

void task2_observer()
{
    std::weak_ptr<int> watcher;
    {
        // 创建一个被 shared_ptr 管理的 int 数据 (值为 42)
        auto data = std::make_shared<int>(42);

        // 将其赋值给 watcher
        watcher = data;

        // 模拟监控
        monitorData(watcher); // 此时应该打印 42

    } // data 离开作用域，int 42 被销毁

    // 再次监控
    monitorData(watcher); // 此时应该打印 expired
}

int main()
{
    std::cout << "--- Task 1: Cycle Reference ---" << std::endl;
    {
        auto node1 = std::make_shared<ImuNode>(1);
        auto node2 = std::make_shared<ImuNode>(2);

        // 构建循环引用：node2 的前驱指向 node1
        node2->prev = node1;
        // 如果不修改 prev 类型，这里离开作用域后，两个 node 都不会销毁
    }
    // 期望输出：Node 2 destroyed, Node 1 destroyed

    std::cout << "\n--- Task 2: Observer ---" << std::endl;
    task2_observer();

    return 0;
}
