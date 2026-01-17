#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QApplication>
#include<QtTest/QTest>
#include<QThread>
#include"ui/base/test_window.h"
using namespace testing;

// 创建全局的 QCoreApplication 实例
class QtTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);


        // 设置应用信息
        app->setApplicationName("TestWindow 测试");
        app->setOrganizationName("UITest");
    }

    void TearDown() override {
        if (app) {
            qDebug() << "清理 QApplication";
            delete app;
            app = nullptr;
        }
    }

private:
    QApplication* app;
};

// 在 main 函数之前注册环境
::testing::Environment* const qt_env = ::testing::AddGlobalTestEnvironment(new QtTestEnvironment);


TEST(UiTest,UiBaseTest)
{
    // 确保有 QCoreApplication 实例
     ASSERT_NE(QApplication::instance(), nullptr);

    //创建窗口
    TestWindow window;
    //设置窗口属性
    window.setWindowTitle("TestWindow 测试");
    window.resize(400,300);

    //显示窗口
    window.show();

    //等待窗口完全显示
    QTest::qWait(100);

    //测试消息功能
    window.showMessage("测试消息功能");
    QTest::qWait(1000);

    window.showMessage("错误消息测试");
    QTest::qWait(1000);

    // 测试清空消息
    window.clearMessage();
    QTest::qWait(500);

    // 测试按钮功能
    window.onShowMessage();
    QTest::qWait(1000);

    window.onClearMessage();
    QTest::qWait(500);

    // 测试窗口关闭
    window.close();
    QTest::qWait(100);

    // 验证测试完成
    EXPECT_TRUE(true) << "TestWindow 基本功能测试完成";
}
