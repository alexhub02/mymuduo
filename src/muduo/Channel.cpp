#include "Channel.h"
#include "TimeStamp.h"
#include "Logger.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{

}

Channel::~Channel()
{

}
// 该方法何时被调用
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true; // 弱指针已绑定强指针
}

// 改变fd感兴趣的事件后,在poller中调用epoll_ctl
void Channel::update()
{
    // 通过Channel所属的EventLoop,调用poller的相关方法,修改epoll上注册的事件
    // loop_->updateChannel(this);
}
// 在Channel所属的EventLoop中删除当前的Channel
void Channel::remove()
{
    // loop_->removeChannel(this);
}
// fd得到Poller通知以后,处理事件
void Channel::handleEvent(TimeStamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (tied_) {
        guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    }
    else {
        handleEventWithGuard(receiveTime);
    }
}
// 根据poller通知的Channel发生的具体事件,执行对应的回调
void Channel::handleEventWithGuard(TimeStamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}