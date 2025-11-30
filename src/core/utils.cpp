#include"utils.h"
using namespace CoreUtils;
bool StringUtils::safeStringCopy(char* dest,const QString& src,size_t destSize)
{
    if(dest==nullptr || destSize==0)
    {
        return false;
    }
    //将QString转换为uft-8编码的QByteArray
    QByteArray utf8Data=src.toUtf8();
    size_t bytesNeeded=utf8Data.size()+1;   //+1用于null终止符

    if(bytesNeeded>destSize)
    {
        //目标缓冲区太小，拷贝尽可能多的数据但保证null停止
        if(destSize>0)
        {
            std::memcpy(dest,utf8Data.constData(),destSize-1);
            dest[destSize-1]='\0';
        }
        return false;
    }
    //缓冲区足够，正常拷贝
    std::memcpy(dest,utf8Data.constData(),bytesNeeded-1);
    dest[bytesNeeded-1]='\0';   //确保null终止
    return true;
}
bool StringUtils::isBlank(const QString& str)
{
    if(str.isEmpty())
    {
        return true;
    }
    //使用QString的trimmed的方法移除首尾空白后检查是否为空
    return str.trimmed().isEmpty();
}
QString StringUtils::truncate(const QString& str,int maxLength)
{
    if(maxLength<0)
    {
        return QString();
    }
    if(str.length()<=maxLength)
    {
        return str;
    }
    return str.left(maxLength);
}
QString TimeUtils::currentTimestamp()
{
    QDateTime now=QDateTime::currentDateTime();
    return now.toString("yyyy-MM-dd hh:mm:ss.zzz");
}
QString TimeUtils::formatLastLoginTime(const QDateTime &loginTime)
{
    if (!loginTime.isValid()) {
        return QString("首次登录");
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 secsDiff = loginTime.secsTo(now);

    if (secsDiff < 60) {
        return QString("刚刚登录");
    } else if (secsDiff < 3600) {
        int minutes = secsDiff / 60;
        return QString("%1分钟前登录").arg(minutes);
    } else if (secsDiff < 86400) {
        int hours = secsDiff / 3600;
        return QString("%1小时前登录").arg(hours);
    } else if (secsDiff < 2592000) { // 30天内
        int days = secsDiff / 86400;
        return QString("%1天前登录").arg(days);
    } else {
        return QString("于%1登录").arg(loginTime.toString("yyyy年MM月dd日"));
    }
}



