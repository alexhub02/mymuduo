set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include/muduo)

aux_source_directory(. MUDUO_SRC_LIST)

add_library(muduo_lib SHARED ${MUDUO_SRC_LIST}
        CurrentThread.cpp
        Thread.cpp
        Logger.cpp
        InetAddress.cpp
        Channel.cpp
        TcpServer.cpp
        EventLoop.cpp)
