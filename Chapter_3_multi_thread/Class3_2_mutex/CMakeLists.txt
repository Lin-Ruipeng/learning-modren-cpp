cmake_minimum_required(VERSION 3.20)
project(app)

option(EXAMPLE "build example.cpp" OFF)

if(EXAMPLE)
    add_executable(example src/example.cpp)
else()
    option(T1 "build main1.cpp" OFF)
    option(T2 "build main2.cpp" OFF)
    
    if(T1)
        add_executable(app1 src/main1.cpp)
    endif()
    
    if(T2)
        add_executable(app2 src/main2.cpp)
    endif()

    # 严格检查：必须至少选一个
    if(NOT T1 AND NOT T2)
        message(FATAL_ERROR "请至少指定 T1=ON 或 T2=ON 或 EXAMPLE=ON!")
    endif()
endif()