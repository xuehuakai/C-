#include"util.h"

namespace moxu{
    pid_t GetThread_Id(){
        return syscall(SYS_gettid);  //SYS_gettid  得到的是线程的PID   getpid() 得到的是进程的PID
        //pthread_self是POSIX线程库提供的函数    syscall是系统调用函数
    }

    uint32_t GetFiber_Id(){
        return 2;
    }
}
