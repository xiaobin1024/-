#ifndef MESSAGE_H
#define MESSAGE_H
#include<QtEndian>
#include<QtDebug>

namespace CoreModule {

//消息枚举类型
enum class MsgType:int{
    REGISTER=1,
    LOGIN=2,
    Quit=3,
    SEARCH=4,
    HISTORY=5,
    HEARTBEAT=6,
    COLLECT=7,
    QUERYCOLLECT=8
};

}


#endif // MESSAGE_H
