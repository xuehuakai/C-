#ifndef _LOG_H__
#define _LOG_H__
#include<string>
#include<cstdint>
#include<iostream>
#include<vector>
#include<memory>
#include<list>
#include<fstream>  //文件流
#include<sstream>  //字符串流
#include<time.h>
#include<string.h>
#include<stdio.h>
#include<map>
#include"util.h"
#include"singleton.h"

#include<stdarg.h> //va_start


/*注：DEBUG 是最小的日志级别，因此下属的getlevel()不管是任何级别都是会输出的*/

/*流式的宏*/
#define MOXU_LOG_LEVEL(logger,level) \
	 if(logger->getlevel() <= level) \
			moxu::LogEventWrap(moxu::LogEvent::ptr(new moxu::LogEvent(logger,\
				level,__FILE__,__LINE__,0,moxu::GetThread_Id(),moxu::GetFiber_Id(),\
				time(NULL)))).getSS()


#define MOXU_LOG_DEBUG(logger) MOXU_LOG_LEVEL(logger,moxu::LogLevel::DEBUG)
#define MOXU_LOG_INFO(logger) MOXU_LOG_LEVEL(logger,moxu::LogLevel::INFO)
#define MOXU_LOG_WARN(logger) MOXU_LOG_LEVEL(logger,moxu::LogLevel::WARN)
#define MOXU_LOG_ERROR(logger) MOXU_LOG_LEVEL(logger,moxu::LogLevel::ERROR)
#define MOXU_LOG_FATAL(logger) MOXU_LOG_LEVEL(logger,moxu::LogLevel::FATAL) /*致命*/

#define MOXU_LOG_FMT_LEVEL(logger,level,fmt,...)\
		if(logger->getlevel()<=level) \
			moxu::LogEventWrap(moxu::LogEvent::ptr(new moxu::LogEvent(logger,level,\
					__FILE__,__LINE__,0,moxu::GetThread_Id(),\
						moxu::GetFiber_Id(),time(0)))).getEvent()->format(fmt,__VA_ARGS__)

#define MOXU_LOG_FMT_DEBUG(logger,fmt,...) MOXU_LOG_FMT_LEVEL(logger,moxu::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define MOXU_LOG_FMT_INFO(logger,fmt,...) MOXU_LOG_FMT_LEVEL(logger,moxu::LogLevel::INFO,fmt,__VA_ARGS__)
#define MOXU_LOG_FMT_WARN(logger,fmt,...) MOXU_LOG_FMT_LEVEL(logger,moxu::LogLevel::WARN,fmt,__VA_ARGS__)
#define MOXU_LOG_FMT_ERROR(logger,fmt,...) MOXU_LOG_FMT_LEVEL(logger,moxu::LogLevel::ERROR,fmt,__VA_ARGS__)
#define MOXU_LOG_FMT_FATAL(logger,fmt,...) MOXU_LOG_FMT_LEVEL(logger,moxu::LogLevel::FATAL,fmt,__VA_ARGS__)


#define MOXU_LOG_ROOT() moxu::LoggerMgr::GetInstance()->getRoot()

namespace moxu{

		//提前声明 
		class Logger;
		//日志级别
		class LogLevel{
				public:
						//日志级别
						enum Level{
								DEBUG=1,
								INFO,
								WARN,
								ERROR,
								FATAL
						}; 
						static const char* ToString(LogLevel::Level level);
		};

		//日志事件
		class LogEvent{
				public:
						typedef std::shared_ptr<LogEvent> ptr;
						LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,
							int32_t line,uint32_t elapse,uint32_t thread_id,
								uint32_t fiber_id,uint64_t time);
						~LogEvent();

						const char* getFile() const {return m_file; }
						int32_t getLine() const {return m_line; }
						uint32_t getElapse() const {return m_elapse; }
						uint32_t getthread_id() const { return m_threadid; }
						uint32_t getfiber_id() const { return m_fiberid; }
						uint32_t gettime() const { return m_time; }
						std::string getcontent() const { return m_ss.str(); }

						std::stringstream & getSS()  { return m_ss; }
						std::shared_ptr<Logger> getLogger() const { return m_logger; }
						LogLevel::Level getLevel() const { return m_level; }

						void format(const char* fmt,...);
						void format(const char* fmt,va_list al); 
				private:
						const char* m_file=nullptr; //文件名
						int32_t m_line=0;//行号
						uint32_t m_elapse=0; //程序启动开始到现在的毫秒数
						uint32_t m_threadid=0;//线程id
						uint32_t m_fiberid=0;//协程号id
						uint32_t m_time;//时间戳
						std::stringstream m_ss;

						std::shared_ptr<Logger> m_logger;
						LogLevel::Level m_level;
		};
	/*这里用wrap的原因是,wrap作为临时对象，在使用完后直接析构，触发日志写入，然而日志本身的shared_ptr，如果声明在主函数中
		程序不结束，就永远无法释放    也是为了简化程序的构造  不然每次日志的处理输出都需要进行显示的event ptr的创建 
		不进行释放就会导致内存泄漏 内存碎片化*/
		class LogEventWrap{  //wrap --包装
			public:
				LogEventWrap(LogEvent::ptr e);
				~LogEventWrap();
				moxu::LogEvent::ptr getEvent() const { return m_event; }
				std::stringstream& getSS();
			private:
				LogEvent::ptr m_event;
		};


		//日志格式器
		class LogFormatter{
				public:
						typedef std::shared_ptr<LogFormatter> ptr;
						LogFormatter(const std::string & pattern); //根据pattern的格式解析items的信息

						std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
				public:
						class FormatItem{ //日志格式项
						public:
							typedef std::shared_ptr<FormatItem> ptr;		
							//FormatItem(const std::string & str){}	
							virtual ~FormatItem() {};
							virtual void  format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
						};

						class MessageFormatItem : public FormatItem{
							public:
								MessageFormatItem(const std::string &str){
									
								}
							 	void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class LevelFormatItem : public FormatItem{
							public:
								LevelFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class ElapseFormatItem : public FormatItem{
							public:
								ElapseFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class ThreadidFormatItem : public FormatItem{
							public:
								ThreadidFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class FiberidFormatItem : public FormatItem{
							public:
								FiberidFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class DateTimeFormatItem : public FormatItem{
							public:
								DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%s")
									:m_format(format){
										if(m_format.empty()){
											m_format="%Y-%m-%d %H:%M:%S";
										}
									} //默认的时间格式
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
							private:
								std::string m_format;
						};

						class FilenameFormatItem : public FormatItem{
							public:
								FilenameFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class LineFormatItem : public FormatItem{
							public:
								LineFormatItem(const std::string &str){

								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class NewLineFormatItem : public FormatItem{
							public:
								NewLineFormatItem(const std::string &str){
									
								}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						class StringFormatItem : public FormatItem{
							public:
								StringFormatItem(const std::string &str)
									:m_string(str){

									}
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
							private:
								std::string m_string;
						};

						class TabFormatItem : public FormatItem{
							public:
								TabFormatItem(const std::string& str=""){} //常量引用绑定到常量对象
								void format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
						};

						void init(); //init做pattern的解析
				private:
						std::string m_pattern; //模式 
						std::vector<FormatItem::ptr> m_items;
		}; 

		//日志输出地
		class LogAppender{
				public:
						typedef std::shared_ptr<LogAppender> ptr;
						virtual ~LogAppender() {} //因为日志输出地有很多 存在继承关系
						virtual	void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0; //纯虚函数 子类必须实现
						void setFormattwer(LogFormatter::ptr val) { m_formatter=val; } //设置Formatter
						LogFormatter::ptr getFormatter() const { return m_formatter; }				

						LogLevel::Level getLevel() const { return m_level; }				

						void setLevel(LogLevel::Level level){ m_level=level; }			
				protected:
						LogFormatter::ptr m_formatter;
						LogLevel::Level m_level=LogLevel::DEBUG;
		};


		//日志器
		class Logger : public std::enable_shared_from_this<Logger>{
				public:
						typedef std::shared_ptr<Logger> ptr;
						Logger(const std::string name= "root" );
						void log(LogLevel::Level level,LogEvent::ptr event);
						void debug(LogEvent::ptr event);
						void debug(LogLevel::Level level,LogEvent::ptr event);
						void info(LogEvent::ptr event);
						void warn(LogEvent::ptr event);
						void error(LogEvent::ptr event);
						void fatal(LogEvent::ptr event);
						//添加Appender
						void addAppender(LogAppender::ptr appender);
						void delAppender(LogAppender::ptr appender);
						LogLevel::Level getlevel() const{ return m_level;  }
						void setlevel(LogLevel::Level val) { m_level=val; }

						const std::string get_name() const { return m_name; }
				private:
						std::string m_name;     					    //日志名称
						LogLevel::Level m_level;					    //满足这个级别的日志才会输出到里面去
						std::list<LogAppender::ptr> m_apperders;        //Appender集合
						LogFormatter::ptr m_formatter;
		};

		//输出到控制台的Appender
		class StdoutLogAppender : public LogAppender{
		public:
				typedef std::shared_ptr<StdoutLogAppender> ptr;//StdoutLogAppender实例化对象的ptr  ptr是类型
				 virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;//参数1：日志级别  参数2：日志事件											 

		};
		//输出到文件的Appender
		class FileLogAppender : public LogAppender{
		public:
				typedef std::shared_ptr<FileLogAppender> ptr;
				 virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
				 FileLogAppender(const std::string filename);
				bool reopen(); //如果文件已经打开，我们进行关闭并再次打开  重新打开成功返回true 

		private:
				 std::ofstream m_filestream;
				 std::string m_filename;
		};

		//日志管理器 --；管理所有的logger  eg.创建logger

		class LoggerManager{
			public:
				LoggerManager(){
					m_root.reset(new Logger); //reset是智能指针本身的成员函数

					m_root->addAppender(LogAppender::ptr(new StdoutLogAppender()));
				}
				Logger::ptr getLogger(const std::string& name){
					auto it =m_loggers.find(name);
					if(it==m_loggers.end()){
					m_loggers.insert({name,m_root});
					std::cout<<"Insert is success"<<std::endl;
					}
					return it==m_loggers.end()? m_root:it->second; //找不到就返回m_root
				}
				Logger::ptr getRoot() const { return m_root; }
				void init();
			private:
				std::map<std::string,Logger::ptr> m_loggers;
				Logger::ptr m_root;
		};

	typedef moxu::Singleton<LoggerManager> LoggerMgr;

}

#endif
 