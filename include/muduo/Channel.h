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
    // fd得到poller通知以后,处理事件的
    void handleEvent(TimeStamp receiveTime);
    // 设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }
    // 防止channel被手动remove掉,channel还在执行回调操作,这块不太理解
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    // 设置fd相应事件掉状态,update是操作poller中对fd的监听
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    // 检查fd当前在poller注册的感兴趣的事件
    bool isNoneEvent() { return events_ == kNoneEvent; }
    bool isWriting() { return events_ == kWriteEvent; }
    bool isReading() { return events_ == kReadEvent; }

    int index() { return index_; }
    void setIndex(int idx) { index_ = idx; }

    // one loop per thread
    EventLoop* ownerLoop() { return loop_; }
    void remove();
private:
    void update();
    void handleEventWithGuard(TimeStamp receiveTime);
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

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

};
#endif //MYMUDUO_CHANNEL_H
