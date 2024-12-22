#ifndef BASE_UTIL_H
#define BASE_UTIL_H

#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>

class StreamingException : public std::runtime_error
{
public:
    // ���캯����������������һ�� std::string ���͵� filename ��һ�� base_uint32_t ���͵�ֵ
    // ���캯����ʼ������ std::runtime_error
    // ����ʼ��һ����Ա���� ss_������һ�� std::auto_ptr<std::stringstream> ���͵�����ָ�룬ָ��һ�� std::stringstream ����
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

// ����궨����һ�� THROW�������׳�һ�� StreamingException �쳣�������ݵ�ǰ�ļ������к���Ϊ������
// ����� __FILE__ �� __LINE__ ��Ԥ��������������ú꣬���ڻ�ȡ��ǰ�ļ������кš�
#define THROW throw StreamingException(__FILE__, __LINE__) \

// ����궨����һ�� CHECK��������һ�� condition ������
// ��� condition Ϊ�� (false)�������׳�һ�� StreamingException �쳣�������������Ϣ��
// #condition ��һ���ַ����������������Ὣ condition ת��Ϊ�ַ�����
#define CHECK(condition) if (!(condition)) \
    THROW << "CHECK FAILED: '" << #condition << "'"

// ����궨����һ�� ASSERT�������ڵ���ģʽ�½��ж��Լ�飺
// ��������� _DEBUG��ͨ���ڵ���ģʽ�¶��壩��ASSERT ����� condition �Ƿ�Ϊ�١����Ϊ�٣����׳�һ�� StreamingException �쳣�������������Ϣ��
// ���û�ж��� _DEBUG��ͨ���ڷ���ģʽ�£���ASSERT �꽫ʲôҲ������
#ifdef _DEBUG
    #define ASSERT(condition) if (!(condition)) \
        THROW << "ASSERT FAILED: '" << #condition << "'"
#else
  #define ASSERT(condition) {}
#endif // DEBUG

std::string getErrorMessage();

#endif // BASE_UTIL_H