#include <iostream>
#include "TimeStamp.h"
#include <thread>
#include <sys/syscall.h>
#include <unistd.h>
#include "Thread.h"
#include "InetAddress.h"
__thread int t_tid;
void func1()
{
    t_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    std::cout << "hello from " << t_tid << std::endl;
}

int main() {
    InetAddress addr(8080, "127.0.0.2");
    std::cout << addr.toIpPort() << std::endl;
    return 0;
}
