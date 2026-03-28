#include <memory>
#include <iostream>

struct ImuData
{
    int id;
    ImuData(int i) : id(i) { std::cout << "ImuData " << id << " created.\n"; }
    ~ImuData() { std::cout << "ImuData " << id << " destroyed.\n"; }
};

void demo_weak_ptr()
{
    std::weak_ptr<ImuData> weak_obs; // 初始为空

    {
        auto shared_owner = std::make_shared<ImuData>(1);
        weak_obs = shared_owner; // 观察者赋值，引用计数不增加！

        std::cout << "Inside scope, use_count: " << shared_owner.use_count() << std::endl; // 1

        // [核心用法] 尝试获取使用权
        if (auto locked = weak_obs.lock())
        {
            // lock() 返回一个 shared_ptr
            // 如果对象已死，返回空的 shared_ptr (nullptr)
            // 如果对象存活，返回一个新的 shared_ptr，引用计数 +1
            std::cout << "Successfully locked data id: " << locked->id << std::endl;
        }
        else
        {
            std::cout << "Data expired." << std::endl;
        }

    } // shared_owner 离开作用域，对象销毁

    // [检测存活]
    if (weak_obs.expired()) // expired 过期,也就是被销毁了
    {
        std::cout << "Data has been destroyed (expired)." << std::endl;
    }

    // [再次尝试 lock]
    if (auto locked = weak_obs.lock())
    {
        // 不会执行
    }
    else
    {
        std::cout << "Lock failed, data is gone." << std::endl;
    }
}

auto main() -> int
{
    demo_weak_ptr();
}