#include<gtest/gtest.h>
#include"network/network_manager.h"
#include <QCoreApplication>
#include<QtTest/QTest>
// 创建全局的 QCoreApplication 实例
class QtTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QCoreApplication(argc, argv);
    }

    void TearDown() override {
        delete app;
    }

private:
    QCoreApplication* app;
};

// 在 main 函数之前注册环境
::testing::Environment* const qt_env = ::testing::AddGlobalTestEnvironment(new QtTestEnvironment);
TEST(NetworkManagerTest, NetworkManagerIsConnectedTest)
{
    // 直接创建对象，不使用fixture
    NetworkManager* manager = new NetworkManager();

    // 测试初始状态应该是未连接
    EXPECT_FALSE(manager->isConnected());

    delete manager;
}

TEST(NetworkManagerTest,NetworkManagerConnectToServerTest)
{
    NetworkManager* manager = new NetworkManager();
    manager->connectToServer("",8080);
    EXPECT_FALSE(manager->isConnected());

    manager->connectToServer("127.0.0.1",-1);
    EXPECT_FALSE(manager->isConnected());
    delete manager;
}

TEST(NetworkManagerTest,NetworkManagerDisconnectFromServerTest)
{
    //测试在未链接状态下断开连接
    NetworkManager* manager = new NetworkManager();
    EXPECT_NO_THROW(manager->disconnectFromServer());
    delete manager;
}

TEST(NetworkManagerTest,NetworkManagerConnectTest)
{
    // 确保有 QCoreApplication 实例
    ASSERT_NE(QCoreApplication::instance(), nullptr);
    //测试连接服务器断开服务器
    NetworkManager* manager = new NetworkManager();

    QObject::connect(manager, &NetworkManager::connectionEstablished, manager, []() {
        qDebug() << "Main: Connection established!";
    });

    QObject::connect(manager, &NetworkManager::networkError, manager, [](const QString &error) {
        qDebug() << "Main: Network error:" << error;
    });

    QObject::connect(manager,&NetworkManager::connectionLost,manager,[](){
        qDebug() << "Main: Network disconnected";
    });

    //连接服务器
    manager->connectToServer("192.168.23.132",8080);
    //等待建立连接
    QTest::qWait(1000);
    //验证连接状态
    EXPECT_TRUE(manager->isConnected());
    //断开连接
    manager->disconnectFromServer();
    //等待断开时间
    QTest::qWait(1000);
    //验证断开状态
    EXPECT_FALSE(manager->isConnected());
    delete manager;
}
