#include <iostream>
#include <memory>

class VisionNode;

class SensorHub
{
private:
    std::shared_ptr<VisionNode> node_;

public:
    SensorHub() { std::cout << "SensorHub 被构造!" << std::endl; }
    ~SensorHub() { std::cout << "SensorHub 被析构!" << std::endl; }
    void bindNode(std::shared_ptr<VisionNode> node) { node_ = node; }
};

class VisionNode
{
private:
    std::weak_ptr<SensorHub> hub_;

public:
    VisionNode() { std::cout << "VisionNode 被构造!" << std::endl; }
    ~VisionNode() { std::cout << "VisionNode 被析构!" << std::endl; }
    void bindHub(std::shared_ptr<SensorHub> hub) { hub_ = hub; }
    void reportStatus()
    {
        if (auto h = hub_.lock()) // 尝试将weak指针升格为shared
        {
            std::cout << "[Node] Hub 连接正常, 准备上报..." << std::endl;
        }
        else
        {
            std::cout << "[Node] Hub 已经挂了!" << std::endl;
        }
    }
};

auto main() -> int
{
    // 1. 创建两节点
    auto hub = std::make_shared<SensorHub>();
    auto node = std::make_shared<VisionNode>();

    // 2. 互相绑定
    hub->bindNode(node);
    node->bindHub(hub);

    // 3. 验证绑定
    node->reportStatus();

    // 4. 运行检查析构函数是否正常执行
    std::cout << "[main] 准备离开作用域..." << std::endl;

    {
        std::cout << "进入局部作用域..." << std::endl;

        // 1. 创建两节点
        auto hub = std::make_shared<SensorHub>();
        auto node = std::make_shared<VisionNode>();

        // 2. 互相绑定
        hub->bindNode(node);
        node->bindHub(hub);

        // 3. 验证绑定
        node->reportStatus();

        std::cout << "离开局部作用域..." << std::endl;
    }

    return 0;
}
