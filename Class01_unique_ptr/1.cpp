#include <memory>
#include <iostream>

struct ImuData {
    double accel[3];
    double gyro[3];
    long timestamp;
};

void demo_unique_ptr() {
    // [构造] 推荐 make_unique，异常安全且更简洁
    std::unique_ptr<ImuData> p1 = std::make_unique<ImuData>();
    
    // [访问] 类似裸指针，通过 -> 或 *
    p1->timestamp = 1000;
    
    // [重置] 释放当前内存，并接管新的内存（或置空）
    p1.reset(new ImuData{1.0, 2.0, 3.0, 2000}); 
    
    // [释放] 放弃所有权，返回裸指针，p1 变为 nullptr
    // 注意：此时内存需由返回的指针手动管理，慎用
    ImuData* raw_ptr = p1.release(); 
    delete raw_ptr; // 演示需要，工程中尽量避免手动 delete

    // [移动语义] unique_ptr 不可拷贝，但可移动
    // 这也是后续我们将数据从驱动层转移到算法层的核心机制
    std::unique_ptr<ImuData> p2 = std::make_unique<ImuData>();
    // std::unique_ptr<ImuData> p3 = p2; // 编译错误：禁止拷贝
    std::unique_ptr<ImuData> p3 = std::move(p2); // 正确：所有权转移，p2 变为 nullptr
    
    if (!p2) {
        std::cout << "p2 is now nullptr after move." << std::endl;
    }
}

