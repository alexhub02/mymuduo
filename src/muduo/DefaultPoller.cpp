#include "Poller.h"
//#include "EPollPoller.h"

#include <cstdlib>

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 生成poll的实例
    }
    else
    {
        return nullptr;
//        return new EPollPoller(loop); // 生成epoll的实例
    }
}
