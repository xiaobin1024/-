#include <gtest/gtest.h>
#include "core/core.h"

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

// 基础断言
TEST(CoreTests, BasicTest) {
    EXPECT_EQ(1, 1);
}
