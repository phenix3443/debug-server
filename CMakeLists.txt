cmake_minimum_required(VERSION 3.16)  # 确保支持 FetchContent
project(RPC_Network_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)  # 启用调试信息

include(FetchContent)  # 引入 FetchContent 机制

# 下载并集成 Boost
FetchContent_Declare(
    Boost
    URL https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.gz
)

FetchContent_MakeAvailable(Boost)

# 指定 Boost 组件
find_package(Boost REQUIRED COMPONENTS system)

# 头文件路径
include_directories(${Boost_INCLUDE_DIRS})

# 生成 RPC 服务器
add_executable(rpc_server src/rpc_server.cpp)
target_link_libraries(rpc_server ${Boost_LIBRARIES})

# 生成 RPC 客户端
add_executable(rpc_client src/rpc_client.cpp)
target_link_libraries(rpc_client ${Boost_LIBRARIES})

# 输出信息
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")