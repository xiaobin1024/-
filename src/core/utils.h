#ifndef UTILS_H
#define UTILS_H
#include<QString>
#include<QDateTime>
#include<QRegularExpression>
namespace CoreUtils
{
    //字符串工具类
    class StringUtils
    {
    public:
        //安全字符拷贝
        static bool safeStringCopy(char* dest,const QString& src,size_t destSize);
        //检查字符串是否为空或仅包含空白字符
        static bool isBlank(const QString& str);
        //截断字符串到指定长度
        static QString truncate(const QString& str,int maxLength);
    };

    //时间工具类
    class TimeUtils
    {
    public:
        //获取当前时间戳
        static QString currentTimestamp();
        //格式化时间
        static QString formatLastLoginTime(const QDateTime& loginTime);
    };
}
#endif // UTILS_H
