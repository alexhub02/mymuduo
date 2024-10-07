#ifndef MYMUDUO_POLLER_H
#define MYMUDUO_POLLER_H
#include <vector>
#include <unordered_map>
#include "noncopyable.h"
#include "TimeStamp.h"

class Channel;
class EventLoop;

class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller();

    virtual TimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;
    // 判断参数channel是否在当前poller中
    bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;
};
#endif //MYMUDUO_POLLER_H
