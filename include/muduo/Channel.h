#ifndef MYMUDUO_CHANNEL_H
#define MYMUDUO_CHANNEL_H
#include "noncopyable.h"
#include <functional>
#include <memory>
/*
 * 封装了fd和其感兴趣的事件,如EPOLLIN、EPOLLOUT
 * */
class EventLoop;
class TimeStamp;
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    void handleEvent(TimeStamp receiveTime);
private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    int events_; // 注册fd感兴趣的事件
    int revents_; // poller返回的具体发生的事件,channel可以根据该变量确定调用的回调函数
    int index_;
    std::weak_ptr<void> tie_;
    bool tied_;

    EventCallback readCallback_;

};
#endif //MYMUDUO_CHANNEL_H
