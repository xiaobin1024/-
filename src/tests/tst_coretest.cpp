#include <gtest/gtest.h>
#include "core/core.h"
#include"core/message.h"
using namespace CoreModule;

// 基础函数测试
TEST(CoreTests, CalculateSumTest) {
    EXPECT_EQ(calculateSum(2, 3), 5);
}

// CoreHandler测试
TEST(CoreTests, CoreHandlerBasicTest) {
    CoreHandler handler;

    // 测试默认状态
    EXPECT_FALSE(handler.isReady());

    // 测试设置状态
    handler.setReady(true);
    EXPECT_TRUE(handler.isReady());
}


TEST(CoreTests,CoreMessageTest)
{
    //消息类型测试，验证每个枚举成员的数值是否符合定义
    EXPECT_EQ(static_cast<int>(MsgType::REGISTER),1);
    EXPECT_EQ(static_cast<int>(MsgType::LOGIN),2);
    EXPECT_EQ(static_cast<int>(MsgType::Quit),3);
    EXPECT_EQ(static_cast<int>(MsgType::SEARCH),4);
    EXPECT_EQ(static_cast<int>(MsgType::HISTORY),5);
    EXPECT_EQ(static_cast<int>(MsgType::HEARTBEAT),6);
    EXPECT_EQ(static_cast<int>(MsgType::COLLECT),7);
    EXPECT_EQ(static_cast<int>(MsgType::QUERYCOLLECT),8);
    //验证MsgType类型是否为int
    EXPECT_EQ(sizeof(MsgType),sizeof(int));

}
