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


// TEST(UiTest,UiBaseTest)
// {
//     // 确保有 QCoreApplication 实例
//      ASSERT_NE(QApplication::instance(), nullptr);

//     //创建窗口
//     TestWindow window;
//     //设置窗口属性
//     window.setWindowTitle("TestWindow 测试");
//     window.resize(400,300);

//     //显示窗口
//     window.show();

//     //等待窗口完全显示
//     QTest::qWait(100);

//     //测试消息功能
//     window.showMessage("测试消息功能");
//     QTest::qWait(1000);

//     window.showMessage("错误消息测试");
//     QTest::qWait(1000);

//     // 测试清空消息
//     window.clearMessage();
//     QTest::qWait(500);

//     // 测试按钮功能
//     window.onShowMessage();
//     QTest::qWait(1000);

//     window.onClearMessage();
//     QTest::qWait(500);

//     // 测试窗口关闭
//     window.close();
//     QTest::qWait(100);

//     // 验证测试完成
//     EXPECT_TRUE(true) << "TestWindow 基本功能测试完成";
// }


// TEST(UiTest,BasicFunctionality)
// {
//     // 确保有 QApplication 实例
//     ASSERT_NE(QApplication::instance(), nullptr);
//     qDebug() << "开始测试 TestWindow 基本功能";

//     // 创建窗口
//     TestWindow window;

//     // 设置窗口属性
//     window.setWindowTitle("TestWindow 功能测试");
//     window.resize(600, 500);

//     // 显示窗口
//     window.show();

//     // 等待窗口完全显示
//     QTest::qWait(200);
//     qDebug() << "窗口已显示";

//     // 测试窗口标题
//     EXPECT_EQ(window.windowTitle(), "TestWindow 功能测试");

//     // 测试窗口尺寸
//     EXPECT_EQ(window.width(), 600);
//     EXPECT_EQ(window.height(), 500);

//     // 等待用户看到窗口
//     qDebug() << "等待2秒，观察窗口外观...";
//     QTest::qWait(2000);

//     // 测试消息功能
//     qDebug() << "测试消息功能...";
//     window.showMessage("这是一条普通测试消息");
//     QTest::qWait(1500);

//     window.showMessage("这是一条错误消息", true);
//     QTest::qWait(1500);

//     // 测试输入框功能
//     qDebug() << "测试输入框功能...";
//     // 测试按钮功能
//     qDebug() << "测试显示消息按钮...";
//     window.onShowMessage();
//     QTest::qWait(3000);

//     qDebug() << "测试清除消息按钮...";
//     window.onClearMessage();
//     QTest::qWait(3000);

//     // 测试主题切换
//     qDebug() << "测试主题切换...";

//     // 记录当前主题
//     UITheme initialTheme = window.uiTheme();
//     qDebug() << "当前主题：" << (initialTheme == UITheme::Light ? "亮色" : "暗色");

//     // 切换到暗色主题
//     window.setUITheme(UITheme::Dark);
//     EXPECT_EQ(window.uiTheme(), UITheme::Dark);
//     qDebug() << "已切换到暗色主题";
//     QTest::qWait(3000);

//     // 切换回亮色主题
//     window.setUITheme(UITheme::Light);
//     EXPECT_EQ(window.uiTheme(), UITheme::Light);
//     qDebug() << "已切换回亮色主题";
//     QTest::qWait(3000);

//     // 测试加载指示器
//     qDebug() << "测试加载指示器...";
//     window.showLoading("正在处理请求...");
//     QTest::qWait(3000);

//     window.hideLoading();
//     qDebug() << "加载指示器已隐藏";
//     QTest::qWait(3000);

//     // 关闭窗口
//     qDebug() << "关闭窗口...";
//     window.close();
//     QTest::qWait(200);

//     EXPECT_TRUE(true) << "TestWindow 基本功能测试完成";
//     qDebug() << "TestWindow 基本功能测试完成";
// }

// TEST(TestWindow, LoadingTest)
// {
//     // 确保有 QApplication 实例
//     ASSERT_NE(QApplication::instance(), nullptr);

//     qDebug() << "\n=== 开始加载指示器测试 ===";

//     // 创建窗口
//     TestWindow window;
//     window.setWindowTitle("加载指示器测试");
//     window.resize(600, 500);
//     window.show();

//     QTest::qWait(300);

//     // 确保窗口可见
//     ASSERT_TRUE(window.isVisible());

//     // 测试1: 显示加载指示器
//     qDebug() << "测试1: 显示加载指示器";
//     window.showLoading("正在处理请求，请稍候...");

//     // 等待并验证加载指示器显示
//     QTest::qWait(1000);
//     qDebug() << "等待1秒，检查加载指示器...";

//     // 测试2: 加载状态下执行其他操作
//     qDebug() << "测试2: 在加载状态下显示消息";
//     window.showMessage("这条消息应该在加载指示器后面");
//     QTest::qWait(2000);

//     // 测试3: 隐藏加载指示器
//     qDebug() << "测试3: 隐藏加载指示器";
//     window.hideLoading();
//     QTest::qWait(3000);
// }

TEST(TestWindow, InteractiveTest)
{
    // 确保有 QApplication 实例
    ASSERT_NE(QApplication::instance(), nullptr);

    qDebug() << "\n开始交互式测试...";

    // 创建窗口
    TestWindow window;
    window.setWindowTitle("交互式测试");
    window.resize(600, 500);
    window.show();

    // 等待窗口显示
    QTest::qWait(300);

    qDebug() << "=== 测试场景 1: 各种消息测试 ===";

    // 测试短消息
    window.showMessage("短消息测试",false,1000);
    QTest::qWait(1500);

    // 测试长消息
    window.showMessage("这是一条非常长的消息，用来测试消息框的换行功能。"
                       "当消息内容很长时，它应该能够自动换行显示。",false,1000);
    QTest::qWait(2000);

    // 测试错误消息
    window.showMessage("网络连接错误：无法连接到服务器！", true,1000);
    QTest::qWait(2000);

    // 清空消息
    window.clearMessage();
    QTest::qWait(5000);

    qDebug() << "=== 测试场景 2: 主题切换测试 ===";

    // 快速切换主题，观察变化
    for (int i = 0; i < 3; ++i) {
        if (window.uiTheme() == UITheme::Light) {
            window.setUITheme(UITheme::Dark);
            window.showMessage("已切换到暗色主题");
        } else {
            window.setUITheme(UITheme::Light);
            window.showMessage("已切换到亮色主题");
        }
        QTest::qWait(1000);
    }

    window.clearMessage();
    QTest::qWait(500);

    qDebug() << "=== 测试场景 3: 加载状态测试 ===";

    // 模拟加载过程
    window.showLoading("正在加载数据...");
    QTest::qWait(1500);

    window.showMessage("数据加载完成");
    //QTest::qWait(2000);

    window.hideLoading();
    //QTest::qWait(2000);

    window.clearMessage();
    QTest::qWait(3000);

    qDebug() << "=== 测试场景 4: 按钮功能测试 ===";

    // 模拟按钮点击
    for (int i = 1; i <= 3; ++i) {
        window.onShowMessage();
        qDebug() << "点击显示消息按钮" << i;
        QTest::qWait(800);

        window.onClearMessage();
        qDebug() << "点击清除消息按钮" << i;
        QTest::qWait(800);
    }
    qDebug() << "交互式测试完成，窗口即将关闭...";
    QTest::qWait(1000);

    window.close();
    QTest::qWait(200);

    EXPECT_TRUE(true) << "交互式测试完成";
    qDebug() << "交互式测试完成";
}
TEST(TestWindow, LayoutAndStyleTest)
{
    // 确保有 QApplication 实例
    ASSERT_NE(QApplication::instance(), nullptr);

    qDebug() << "\n开始布局和样式测试...";

    // 创建多个窗口实例，测试不同尺寸
    qDebug() << "测试不同窗口尺寸下的布局...";

    // 小窗口
    {
        TestWindow smallWindow;
        smallWindow.setWindowTitle("小窗口测试");
        smallWindow.resize(300, 200);
        smallWindow.show();

        qDebug() << "显示小窗口 (300x200)";
        QTest::qWait(1500);
        smallWindow.close();
    }

    QTest::qWait(500);

    // 中等窗口
    {
        TestWindow mediumWindow;
        mediumWindow.setWindowTitle("中等窗口测试");
        mediumWindow.resize(500, 400);
        mediumWindow.show();

        qDebug() << "显示中等窗口 (500x400)";
        QTest::qWait(1500);
        mediumWindow.close();
    }

    QTest::qWait(500);

    // 大窗口
    {
        TestWindow largeWindow;
        largeWindow.setWindowTitle("大窗口测试");
        largeWindow.resize(800, 600);
        largeWindow.show();

        qDebug() << "显示大窗口 (800x600)";

        // 测试消息在大窗口中的显示
        largeWindow.showMessage("这是一个在大窗口中显示的长消息，用于测试布局的自适应能力。"
                                "窗口大小变化时，消息框应该能够正确调整。");
        QTest::qWait(2000);

        // 切换主题
        largeWindow.setUITheme(UITheme::Dark);
        QTest::qWait(1500);

        largeWindow.setUITheme(UITheme::Light);
        QTest::qWait(1500);

        largeWindow.close();
    }

    qDebug() << "布局和样式测试完成";
    EXPECT_TRUE(true) << "布局和样式测试完成";
}
