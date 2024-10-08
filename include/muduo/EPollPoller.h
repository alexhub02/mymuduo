#ifndef MYMUDUO_EPOLLPOLLER_H
#define MYMUDUO_EPOLLPOLLER_H
#include "Poller.h"
#include "TimeStamp.h"
#include <sys/epoll.h>
#include <vector>

class EventLoop;
class Channel;
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;
    // override关键字可以在派生类未重写基类的虚函数时,在编译时报错
    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel通道,实现fd在epoll内核上注册的感兴趣事件的修改
    void update(int opt, Channel *channel);

    int epollfd_;
    using EventList = std::vector<epoll_event>;
    EventList events_;
};
#endif //MYMUDUO_EPOLLPOLLER_H
