#ifndef _MYLOG_H
#define _MYLOG_H

#include<log4cpp/Category.hh>
#include<iostream>

//日志优先级
enum Priority {
    ERROR,
    WARN,
    INFO,
    DEBUG
};

//用单例模式封装log4cpp
class Mylog {
 public: 
    static Mylog& getInstance();
    static void destory();

    void setPriority(Priority priority);

template<class ...Args>
    void error(const char* msg,Args ... args){
    category_ref_.error(msg,args...);
    }
template<class ...Args>
    void warn(const char* msg,Args ... args){
    category_ref_.warn(msg,args...);

    }
template<class ...Args>
    void info(const char* msg,Args ... args){
    category_ref_.info(msg,args...);

    }
template<class ...Args>
    void debug(const char* msg,Args ... args){
    category_ref_.debug(msg,args...);

    }

 private:
    Mylog();  //单例模式：构造函数私有化

 private:
    static Mylog *plog_;
    // log4cpp 中的category类构造函数是protected, 只能通过引用访问
    log4cpp::Category& category_ref_;
};


//*****************************************************
//注意：
//文件名 __FILE__ ,函数名 __func__ ，行号__LINE__ 是编译器实现的
//并非C++头文件中定义的 
//前两个变量是string类型，且__LINE__是整形，所以需要转为string类型
//******************************************************

//整数类型文件行号 ->转换为string类型
inline std::string int2string(int line) {
    std::ostringstream oss;
    oss << line;
    return oss.str();
}


//定义一个在日志后添加 文件名 函数名 行号 的宏定义

#define prefix(msg) string("[")\
	.append(__FILE__).append(":")\
	.append(__FUNCTION__).append(":")\
	.append(std::to_string(__LINE__)).append("] ")\
	.append(msg).c_str()

#define LogError(msg, ...) Mylogger::getInstance()->error(prefix(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...)  Mylogger::getInstance()->warn(prefix(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...)  Mylogger::getInstance()->info(prefix(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(prefix(msg), ##__VA_ARGS__)



//不用每次使用时写 getInstance语句
//只需要在主函数文件中写: #define _LOG4CPP_即可在整个程序中使用
#ifdef _LOG4CPP_
Mylog &log = Mylog::getInstance();
#else
extern Mylog &log;
#endif



#endif

