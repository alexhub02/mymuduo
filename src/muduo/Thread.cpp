#include "Thread.h"
#include "CurrentThread.h"
#include <semaphore.h>

#include <memory>

std::atomic<int> Thread::numCreated_(0); // 类型 作用域::变量名

void Thread::setDefaultName()
{
    int num = ++numCreated_; // 先自增1,然后赋值给左边
    if (name_.empty())
    {
        char buffer[32] = {0};
        snprintf(buffer, sizeof buffer, "Thread%d", num);
        name_ = buffer;
    }
}

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false),
      joined_(false),
      tid_(0),
      func_(std::move(func)),
      name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_) {
        // join表示由主线程回收,detach表示由系统回收
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0); // 进程间不共享,初值为0

    thread_ = std::make_shared<std::thread>([&](){
        // 获取线程tid值
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        // 开启一个新线程,执行func_函数
        func_();
    });
    // 等待获取上面新建线程的tid值
    sem_wait(&sem);
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}

