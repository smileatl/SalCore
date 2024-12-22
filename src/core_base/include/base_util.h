#ifndef BASE_UTIL_H
#define BASE_UTIL_H

#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>

class StreamingException : public std::runtime_error
{
public:
    // 构造函数接受两个参数：一个 std::string 类型的 filename 和一个 base_uint32_t 类型的值
    // 构造函数初始化基类 std::runtime_error
    // 并初始化一个成员变量 ss_。其是一个 std::auto_ptr<std::stringstream> 类型的智能指针，指向一个 std::stringstream 对象。
    StreamingException(const std::string filename = "", base_uint32_t = 0) :
        std::runtime_error(""),
        ss_(std::auto_ptr<std::stringstream>(new std::stringstream()))
    {
    }

    ~StreamingException() throw()
    {
    }

    template <typename T>
    StreamingException & operator << (const T & t)
    {
        (*ss_) << t;
        return *this;
    }

    virtual const char * what() const throw()
    {
        s_ = ss_->str();
        return s_.c_str();
    }

public: // fields
    std::string  filename_;
    base_uint32_t line_;

private:
    mutable std::auto_ptr<std::stringstream> ss_;
    mutable std::string s_;
};

// 这个宏定义了一个 THROW，它会抛出一个 StreamingException 异常，并传递当前文件名和行号作为参数。
// 宏参数 __FILE__ 和 __LINE__ 是预处理器定义的内置宏，用于获取当前文件名和行号。
#define THROW throw StreamingException(__FILE__, __LINE__) \

// 这个宏定义了一个 CHECK，它接受一个 condition 参数。
// 如果 condition 为假 (false)，它会抛出一个 StreamingException 异常，并输出错误信息。
// #condition 是一个字符串化操作符，它会将 condition 转换为字符串。
#define CHECK(condition) if (!(condition)) \
    THROW << "CHECK FAILED: '" << #condition << "'"

// 这个宏定义了一个 ASSERT，用于在调试模式下进行断言检查：
// 如果定义了 _DEBUG（通常在调试模式下定义），ASSERT 将检查 condition 是否为假。如果为假，会抛出一个 StreamingException 异常，并输出错误信息。
// 如果没有定义 _DEBUG（通常在发布模式下），ASSERT 宏将什么也不做。
#ifdef _DEBUG
    #define ASSERT(condition) if (!(condition)) \
        THROW << "ASSERT FAILED: '" << #condition << "'"
#else
  #define ASSERT(condition) {}
#endif // DEBUG

std::string getErrorMessage();

#endif // BASE_UTIL_H