cmake_minimum_required(VERSION 3.22)
project(mymuduo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -fPIC")

include_directories(include/muduo)

add_subdirectory(src)

add_executable(mymuduo main.cpp
        include/muduo/TimeStamp.h
        src/muduo/TimeStamp.cpp
        include/muduo/Thread.h
        include/muduo/noncopyable.h
        include/muduo/CurrentThread.h
        include/muduo/Logger.h
        include/muduo/InetAddress.h
        include/muduo/EventLoop.h
        include/muduo/Channel.h
        include/muduo/TcpServer.h
)
target_link_libraries(mymuduo PRIVATE muduo_lib)