#include <gtest/gtest.h>
#include"core/utils.h"
using namespace CoreUtils;

/*
正常情况：字符串长度小于目标缓冲区大小
边界情况：字符串长度等于目标缓冲区大小（包括null终止符）
异常情况：字符串长度大于目标缓冲区大小
异常情况：目标指针为nullptr
 */
TEST(CoreTest,CoreUtilsSafeStringCopyTest)
{
    //测试1，字符串长度小于目标缓冲区大小
    char dest[20];
    QString src="Hello World";
    bool result=StringUtils::safeStringCopy(dest,src,sizeof(dest));
    EXPECT_TRUE(result);
    EXPECT_STREQ(dest,"Hello World");
    //测试1.1空字符串
    char dest2[20];
    result=StringUtils::safeStringCopy(dest2,QString(""),sizeof(dest2));
    EXPECT_TRUE(result);
    EXPECT_STREQ(dest2,"");

    //测试2，字符串长度等于目标缓冲区大小
    char dest3[12];
    result=StringUtils::safeStringCopy(dest3,src,sizeof(dest3));
    EXPECT_TRUE(result);
    EXPECT_STREQ(dest3,"Hello World");

    //测试3，字符串长度大于目标缓冲区大小
     char dest4[5];
    result=StringUtils::safeStringCopy(dest4,src,sizeof(dest4));
    EXPECT_FALSE(result);
    EXPECT_STREQ(dest4,"Hell");

    //测试4，目标指针为nullptr
    result = StringUtils::safeStringCopy(nullptr, src, 10);
    EXPECT_FALSE(result); // 应该返回false，因为目标指针为null

    //测试中文
    src="中文简体";
    char dest5[20];
    result = StringUtils::safeStringCopy(dest5, src, sizeof(dest5));
    EXPECT_TRUE(result);
    EXPECT_STREQ(dest5,"中文简体");

}

//测试字符串是否为空或仅包含空白字符
TEST(CoreTest,CoreUtilsIsBlankTest)
{
    QString blank="";
    bool result=StringUtils::isBlank(blank);
     EXPECT_TRUE(result);

    blank=" ";
    result=StringUtils::isBlank(blank);
    EXPECT_TRUE(result);

    blank="Hello World";
     result=StringUtils::isBlank(blank);
    EXPECT_FALSE(result);
}
//测试截断字符串到指定长度
TEST(CoreTest,CoreUtilsTruncateTest)
{
    QString str="Hello World";
    QString result=StringUtils::truncate(str,5);
    EXPECT_EQ(result,"Hello");

    result=StringUtils::truncate(str,0);
    EXPECT_EQ(result,"");

    result=StringUtils::truncate(str,20);
    EXPECT_EQ(result,str);
}
//测试当前时间戳格式
TEST(CoreTest,CoreUtilsCurrentTimeStampTest)
{
    QString timestamp=TimeUtils::currentTimestamp();
    //验证非空，正确长度
    EXPECT_FALSE(timestamp.isEmpty());
    EXPECT_EQ(timestamp.length(),23);

    //验证基本格式
    EXPECT_TRUE(timestamp.contains("2025"));
    EXPECT_TRUE(timestamp.contains("-"));
    EXPECT_TRUE(timestamp.contains(":"));

    //打印出来
    std::cout<<"当前时间戳："<<timestamp.toStdString()<<std::endl;
}
//测试格式化时间
TEST(CoreTest,CoreUtilsFormatLastLoginTimeTest)
{
    QDateTime now = QDateTime::currentDateTime();
    //测试1，无效时间
    QDateTime invalidTime;
    QString result=TimeUtils::formatLastLoginTime(invalidTime);
    EXPECT_EQ(result,"首次登录");

    //测试2，刚刚登录（59秒前）
    QDateTime justNow=now.addSecs(-59);
    result=TimeUtils::formatLastLoginTime(justNow);
    EXPECT_EQ(result,"刚刚登录");

    //测试3，几分钟前登录（5分钟前）
    QDateTime minutesAgo=now.addSecs(-5*60);
    result=TimeUtils::formatLastLoginTime(minutesAgo);
    EXPECT_EQ(result,"5分钟前登录");

    //测试4，几小时前登录（3小时前）
    QDateTime hoursAgo=now.addSecs(-3*3600);
    result=TimeUtils::formatLastLoginTime(hoursAgo);
    EXPECT_EQ(result,"3小时前登录");

    //测试5，几天前登录（5天前）
    QDateTime daysAgo=now.addSecs(-5*86400);
    result=TimeUtils::formatLastLoginTime(daysAgo);
    EXPECT_EQ(result,"5天前登录");

    //测试6，超过30登录（35天前）
    QDateTime longAgo=now.addSecs(-35*86400);
    result=TimeUtils::formatLastLoginTime(longAgo);
    //验证格式包含年月日
    EXPECT_TRUE(result.startsWith("于"));
    EXPECT_TRUE(result.contains("年"));
    EXPECT_TRUE(result.contains("月"));
    EXPECT_TRUE(result.contains("日"));
    EXPECT_TRUE(result.endsWith("登录"));

    //打印结果查看
    std::cout<<"35天前"<<result.toStdString()<<std::endl;

}


