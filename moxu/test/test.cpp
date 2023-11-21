#include<iostream>
#include"../src/log.h"
#include"../src/util.h"
/*输出是通过Logger本体进行输出的*/
#include<thread>
int main(){
	moxu::Logger::ptr logger(new moxu::Logger);
    logger->addAppender(moxu::LogAppender::ptr(new moxu::StdoutLogAppender));

 //   moxue::LogEvent::ptr event(new moxue::LogEvent(__FILE__,__LINE__,0,1,2,time(0)));

	// std::thread::id thread_id=std::this_thread::get_id();
	// ssize_t thread_hash=std::hash<std::thread::id>{} (thread_id);
	// int ans=static_cast<int>(thread_hash);

	// std::cout<<"The thread_Id is "<<ans<<std::endl;  //错的  输出的只是哈希值

    // moxu::LogEvent::ptr event(new moxu::LogEvent(__FILE__, __LINE__, 0, moxu::GetThread_Id(), 
    //                     moxu::GetFiber_Id(),time(0)));

    // event->getSS()<<"Hello moxue log"<<std::endl;

    // logger->log(moxu::LogLevel::DEBUG,event);


    moxu::FileLogAppender::ptr file_appender(new moxu::FileLogAppender("./log.txt"));


    moxu::LogFormatter::ptr fmt(new moxu::LogFormatter("%d%T%p%T%m%n"));

    file_appender->setFormattwer(fmt);
    file_appender->setLevel(moxu::LogLevel::ERROR); //只存放ERROR级别及以上的日志
    logger->addAppender(file_appender);
                                                         

    MOXU_LOG_INFO(logger) << "Test macro";
    MOXU_LOG_ERROR(logger) <<"Test macro error";
    MOXU_LOG_FATAL(logger) <<"The Web is faile";

    MOXU_LOG_FMT_ERROR(logger,"test macro fmt error %s","aa");


    auto p = moxu::LoggerMgr::GetInstance();

    moxu::Logger::ptr i = p->getLogger("xx");

    auto p2 = moxu::LoggerMgr::GetInstance();


    std::cout<<"测试地址"<<std::endl;
    std::cout<<"p的地址："<<p<<std::endl;
    std::cout<<"p2的地址："<<p2<<std::endl;


    MOXU_LOG_INFO(i)<<"xxx";
    return 0;
}

//做一个日志的管理其，不需要每次自己创建logger
