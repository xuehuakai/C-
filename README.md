# moxu --服务器名字

# 开发环境 linux
cmake
# 项目：
bin --二进制
build --中间文件路径
cmake --cmake函数文件夹
CMakeLists.txt --cmake的定义文件
lib --库的输出路径
src --源代码
test --测试文件

## 日至系统 --首先
Log4J

    Logger(定义日至类别)
      |
      |-------Formatter(日志格式)
      |
    Appender(日至输出地方)

## 配置系统

Config ---> Yaml

```
yaml-app\:github 搜
mkdir build && cd build && cmake .. && cmake install
```
配置系统的原则，约定优先于配置：
```cpp
template\<T,FromStr,ToStr>
class ConfigVar;

template\<F,T>
LexicalCast;
```

容器偏特化,目前支持vector,list,set,map,unordered\_map,unordered\_set, map/umap支持key = std::string的偏特化
config::lookup(key)  key相同，类型不同的，不会有报错

自定义类型，需要实现moxu::LexicalCast,偏特化实现后，就可以支持Config解析自定义类型，自定义类型可以和常规stl容器一起使用，因为我们之前已经将stl偏特化处理过了.

配置的事件机制：
当一个配置发生修改的时候，可以反向通知对应的代码，进行回调

# 日志系统整合配置系统

```yaml
	logs:
    - name: root
      level: info
      formatter: "%d%T%m%n"
      appender:
          - type: FileLogAppender
            file: log.txt
          - type: StdoutLogAppender
    - name: system
      level: debug
      formatter: "%d%T%m%n"
      appender:
          - type: FileLogAppender
            file: log.txt
          - type: StdoutLogAppender  
```

```cpp
	moxu::Logger g_logger = moxu::LoggerMgr::GetInstance()->getLogger(name);
	MOXU_LOG_INFO(g_logger)<<"xxx.log";
```

```cpp
	static Logger::ptr g_log = MOXU_LOG_NAME("system"); //以前在这里拿到的是root
	//现在拿到是system,system没东西，真正还是root，以后当system有东西了就不是roo	//t了

	当logger的appenders为空，使用root写logger
```

```cpp
//定义logDefine LogAppenderDefine,偏特化LexicalCast
//实现了日志的配置解析
```

```cpp
```

## 基于协程 协程库的封装  协程：将异步的操作封装成同步

## websocket开发

## http协议开发

## 分布式协议库  --在socket库的基础上再封装一套分布式协议库

## 推荐系统

