#include"log.h"
#include<ctype.h>
#include<map>
#include<functional>

/*#--宏定义中，将后面的内容进行字符串化*/

namespace  moxu{

		const  char* LogLevel::ToString(LogLevel::Level level){
				switch (level)
				{
		#define XX(name) \
			case LogLevel::name: \
				return #name; \
				break;
				
				XX(DEBUG);
				XX(INFO);
				XX(WARN);
				XX(ERROR);
				XX(FATAL);

		#undef XX
				default:
					return "UNKNOW";
				}
				return "UNKNOW";
		}

	//LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time):
	LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time):
		m_file(file),
		m_elapse(elapse),
		m_fiberid(fiber_id),
		m_line(line),
		m_threadid(thread_id),
		m_time(time),
		m_logger(logger),
		m_level(level)
	{

	}


	void LogEvent::format(const char* fmt,...){
		va_list al;
		va_start(al,fmt);
		format(fmt,al);
		va_end(al);
	}
	void LogEvent::format(const char* fmt,va_list al){
		char* buf=nullptr;
		int len=vasprintf(&buf,fmt,al);//动态分配一个缓冲区buf来存储格式化后的字符串 
		//将要格式化的字符串与可变参数结合起来，将格式化和后的内容存储到buf中
		if(len!=-1){
			m_ss<<std::string(buf,len);
			free(buf);
		}
	}

	LogEventWrap::LogEventWrap(LogEvent::ptr e):m_event(e){
		//std::cout<<"封装Event的构造"<<std::endl;
	}

	LogEventWrap::~LogEventWrap(){
			m_event->getLogger()->log(m_event->getLevel(),m_event); //触发日志写入(logger->log())
	}

	std::stringstream& LogEventWrap::getSS(){
		return m_event->getSS();
	}


	LogEvent::~LogEvent(){
		
	}

		Logger::Logger(const std::string name):m_name(name),m_level(LogLevel::DEBUG){
				m_formatter.reset(new LogFormatter("%d%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
		}
		void Logger::log(LogLevel::Level level,LogEvent::ptr event){
				if(level>=m_level){
						auto self=shared_from_this(); //在自己的成员函数中获取自己的指针
						for(auto & i: m_apperders){
								i->log(self,level,event); //
						}
				}
		}

		void  Logger::debug(LogEvent::ptr event){

				log(LogLevel::DEBUG,event);
		}

		void Logger::info(LogEvent::ptr event){

				log(LogLevel::INFO,event);
		}
		void Logger::warn(LogEvent::ptr event){

				log(LogLevel::WARN,event);
		}
		void Logger::error(LogEvent::ptr event){

				log(LogLevel::ERROR,event);
		}
		void Logger::fatal(LogEvent::ptr event){

				log(LogLevel::FATAL,event);
		}


		void Logger::addAppender(LogAppender::ptr appender){
			if(!appender->getFormatter()){
				appender->setFormattwer(m_formatter); /*如果输出没有Formater的话 将我们自己的formater 保证每一个输出都有
				自己的Formater*/
			}
				m_apperders.push_back(appender);
		}
		void Logger::delAppender(LogAppender::ptr appender){
				for(std::list<LogAppender::ptr>::iterator it=m_apperders.begin();it!=m_apperders.end();++it){
						if(*it==appender){
								m_apperders.erase(it);
								break;
						}
				}
		}

		FileLogAppender::FileLogAppender(const std::string filename):m_filename(filename){
			bool ans=reopen();
		}


		void  StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			if(level>=m_level){
				std::string str=m_formatter->format(logger,level,event);
				std::cout<<str;//<<std::endl;
			}
	
		}
		void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			if(level>=m_level){
				m_filestream << m_formatter->format(logger,level,event)<<std::endl;  
			}
		}

		bool FileLogAppender::reopen(){
			if(m_filestream){ //如果是已经打开的
				m_filestream.close();
			}	
			m_filestream.open(m_filename);
			return !!m_filestream;
		}

		std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
				std::stringstream ss;
				//std::cout<<"此时的m_items的大小为"<<m_items.size()<<std::endl;
				for(auto & i : m_items){
					i->format(ss,logger,level,event);
				}
				return ss.str();
		}

		LogFormatter::LogFormatter(const std::string & pattern)
				:m_pattern(pattern) {
					init();
		}

		void LogFormatter::init(){ //解析日志输出
				//str format type
				std::vector<std::tuple<std::string,std::string,int>> vec; //tuple (string string int)
				std::string nstr;
				for(size_t i=0;i<m_pattern.size();++i){ //m_pattern是一个模式string
					if(m_pattern[i]!='%'){ 
						nstr.append(1,m_pattern[i]);//追加一个字
					    continue;
					}

					if((i+1)<m_pattern.size()){  //%%
						if(m_pattern[i+1]=='%'){
							nstr.append(1,'%');
						continue;
						}
					}

					size_t n=i+1;
					size_t fmt_begin=0;
					int format_status=0;//格式状态
					std::string fmt;
					std::string str;
					while(n<m_pattern.size()){
						if(!std::isalpha(m_pattern[n]) && m_pattern[n]!='{' &&
								m_pattern[n]!='}'){ //只接受字母
							str=m_pattern.substr(i+1,n-i-1);
							break; //日志格式出现非法字符 直接break
						}
						if(format_status == 0){ //尚未遇到{
							if(m_pattern[n] == '{'){
								str=m_pattern.substr(i+1,n-i-1);
								format_status = 1; //解析格式
								fmt_begin=n;
								++n;
								continue; 
							}		
						}

						else if (format_status==1)
						{
							if(m_pattern[n]== '}' ){
								fmt=m_pattern.substr(fmt_begin+1,n-fmt_begin-1);
								format_status=0;   // /   /==2代表{}字符匹配结束
								++n;
								break;
							}
						}
						++n;
						if(n==m_pattern.size()){
							if(str.empty()){
								str=m_pattern.substr(i+1);
							}
						}
					}

					if(format_status==0){
						if(!nstr.empty()){
							vec.push_back(std::make_tuple(nstr,std::string(),0));
							nstr.clear(); //删除nstr  将空间的首字符置为0
						}
						//str=m_pattern.substr(i+1,n-i-1);
						vec.push_back(std::make_tuple(str,fmt,1));
						i=n-1;
					}
					//格式错误
					else if(format_status==1){
						std::cout<<"pattern parse error "<<m_pattern<<" - "<<m_pattern.substr(i)<<std::endl;
						vec.push_back(std::make_tuple("<<pattern_error>>",fmt,1));
					}
					// else if(format_status==2){
					// 	if(!nstr.empty()){
					// 		vec.push_back(std::make_tuple(nstr,"",0));
					// 		nstr.clear();
					// 	}
					// 	vec.push_back(std::make_tuple(str,fmt,1));
					// 	i=n-1;
					// }

				}
				if(!nstr.empty()){
					vec.push_back(std::make_tuple(nstr,"",0));
				}

	
			
			const std::string m="m",p="p",c="c",r="r",t="t",n="n",d="d",l="l",f="f",T="T",F="F";
			static std::map<std::string,std::function<FormatItem::ptr (const std::string & str)>>
			 s_format_items = {
	#define XX(str,C) { \
		str,[](const std::string & fmt){ return FormatItem::ptr(new C(fmt)); } \
		}

				XX(f,FilenameFormatItem),
				XX(m,MessageFormatItem),
				XX(p,LevelFormatItem),
				XX(r,ElapseFormatItem),
				XX(c,FilenameFormatItem),
				XX(t,ThreadidFormatItem),
				XX(n,NewLineFormatItem),
				XX(d,DateTimeFormatItem),
				XX(l,LineFormatItem),
				XX(T,TabFormatItem),
				XX(F,FiberidFormatItem)
	#undef XX //取消定义宏
			};
			/*%m --消息体
			  %p --level
			  %r --启动后的时间
			  %c --日志名称
			  %t --线程id
			  %n --回车换行
			  %d --时间
			  %f --文件名
			  %l --行号*/

		for(auto & i : vec){
			if(std::get<2>(i)==0){ //string
				m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
			}
			else{
				auto it = s_format_items.find(std::get<0>(i));
				if(it == s_format_items.end()){
					m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+
					std::get<0>(i)+">>")));
				}
				else{
					m_items.push_back(it->second(std::get<1>(i)));
				}
			}
			//std::cout<<"{"<<std::get<0>(i)<<"} - {"<<std::get<1>(i)<<"} - {"<<std::get<2>(i)<<"}"<<std::endl;
		}
		//std::cout<<"size="<<m_items.size()<<std::endl;
		
		}

	void LogFormatter::MessageFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)
	{
			os<<event->getcontent();
	}//直接输出event里面

	void LogFormatter::LevelFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<LogLevel::ToString(level);
	}

	void LogFormatter::ElapseFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getElapse();
	}

	void LogFormatter::ThreadidFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getthread_id();
	}
	void LogFormatter::FiberidFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getfiber_id();
	}

	void LogFormatter::DateTimeFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			struct tm tm;
			time_t time=event->gettime();
			localtime_r(&time,&tm);
			char buf[1024];
			strftime(buf,sizeof buf,m_format.c_str(),&tm);

			os<<buf;
	}
	
	void LogFormatter::FilenameFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<event->getFile();
	}

	void LogFormatter::LineFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<" "<<event->getLine();
	}

	void LogFormatter::NewLineFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<std::endl;
	}


	void LogFormatter::StringFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<m_string;
	}


	void LogFormatter::TabFormatItem::format(std::ostream & os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
			os<<"\t";
	}
}
