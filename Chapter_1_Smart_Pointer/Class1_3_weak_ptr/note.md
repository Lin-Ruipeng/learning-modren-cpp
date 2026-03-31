# Class 3 weak_ptr —— 打破循环与观测者模式

## 1.cpp

### 1.创建
```cpp
std::weak_ptr<T> weak_obs; // 初始为空
```

### 2.取得地址
```cpp
weak_obs = shared_owner; // 后面这个是一个shared_ptr
```

### 3.尝试获取所有权
```cpp
if (auto locked = weak_obs.lock()) // 获取成功则为true!
```

### 4.检测过期
```cpp
if (weak_obs.expired()) // 如果过期了就为true!
```

## 2.cpp

背景：
如果有一个“观察者”，它想查看 IMU 数据，但不想阻止数据被销毁，也不想增加引用计数的开销，该怎么办？
这就是 weak_ptr 的用武之地。它是一种“弱引用”，不拥有对象的所有权，但能检测对象是否存活。

场景背景：
你在开发一个 导航状态监控模块。
主程序在一个循环中不断更新 IMU 数据（shared_ptr），而监控线程持有数据的 weak_ptr。监控线程需要定期检查：

数据是否还存在？
如果存在，打印当前的 ID；如果不存在，打印警告。
同时，我们需要解决一个经典的 循环引用 问题。
