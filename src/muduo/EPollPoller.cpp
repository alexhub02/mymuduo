#include "EPollPoller.h"
#include "Logger.h"
#include "TimeStamp.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>
#include <strings.h>

const int kNew = -1; // Channel未添加到Poller中,channels_中也无
const int kAdded = 1; // Channel已添加到Poller中
const int kDeleted = 2; // Channel从Poller中删除,channels_中还有

// 派生类的构造函数,需要先初始化基类
EPollPoller::EPollPoller(EventLoop *loop) :
    Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kInitEventListSize)
{
    if (epollfd_ < 0) {
        LOG_FATAL("epoll_create error : %d\n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

// 填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel); // EventLoop就拿到了它的poller给它返回的所有发生事件的channel列表了
    }
}

TimeStamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_INFO("func=%s => fd total count:%lu\n", __FUNCTION__ , channels_.size());
    // LT模式,未处理的事件会不断上报
    int numEvents = ::epoll_wait(epollfd_, &(*events_.begin()), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    TimeStamp now(TimeStamp::now());

    if (numEvents > 0) {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__);
    } else {
      if (saveErrno != EINTR) {
          errno = saveErrno;
          LOG_ERROR("poll err!\n");
      }
    }
    return now;
}

// Channel update/remove通过调用EventLoop的update/remove方法,实现调用Poller的update/remove方法
/*
 * 如果该fd在epoll内核上未注册,注册
 * 如果该fd在epoll内核上已注册,若fd无感兴趣事件则取消注册,反之修改注册的感兴趣的事件
 * */
void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    LOG_INFO("func=%s => fd = %d events=%d index=%d \n", __FUNCTION__, channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

// 从Poller中删除fd,对应的channel变为kNew
void EPollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("func=%s =>%d",__FUNCTION__, fd);

    int index = channel->index();
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

// 实际调用epoll_ctl实现add/mod/del的接口
void EPollPoller::update(int opt, Channel *channel)
{
    epoll_event event;
    bzero(&event, sizeof event);

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, opt, fd, &event) < 0) {
        if (opt == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}