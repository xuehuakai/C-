#ifndef _MOXU_UTIL_H__
#define _MOXU_UTIL_H__

#include<iostream>
#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<unistd.h>
//#include<zconf.h>


namespace moxu{
    pid_t GetThread_Id();
    uint32_t GetFiber_Id();
}

#endif

