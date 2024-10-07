#ifndef MYMUDUO_CURRENTTHREAD_H
#define MYMUDUO_CURRENTTHREAD_H
#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
    extern __thread int t_cacheTid; // __thread关键字表明该变量为每个线程的私有变量
    void cacheTid();
    inline int tid()
    {
        if(__builtin_expect(t_cacheTid == 0, 0)) {
            cacheTid();
        }
        return t_cacheTid;
    }
}
#endif //MYMUDUO_CURRENTTHREAD_H
