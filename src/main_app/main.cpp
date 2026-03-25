#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QCoreApplication>

// 包含项目中使用的模块
#include "view/page_controller.h"
#include "user_session/user_session.h"
#include "dispatcher/message_dispatcher.h"
#include "network/network_manager.h"
#include "base/base_widget.h"

int main(int argc, char *argv[])
{
    // 设置环境变量帮助调试
    // qputenv("QT_LOGGING_RULES", "*.debug=true");

    qDebug() << "=== 启动 Network Dictionary 应用 ===";
    qDebug() << "应用程序路径:" << QCoreApplication::applicationDirPath();
    qDebug() << "当前工作目录:" << QDir::currentPath();

    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("Network Dictionary");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("NetworkDict");

    qDebug() << "QApplication 创建成功";

    try {
        // 创建主窗口
        BaseWidget *window = new BaseWidget();
        qDebug() << "BaseWidget 创建成功";

        window->resize(1000, 700);
        window->setWindowTitle("Network Dictionary");

        window->initialize();  // 手动调用初始化
        qDebug() << "BaseWidget 初始化完成";

        QApplication::processEvents();

        // 创建 NetworkManager
        NetworkManager *networkManager = new NetworkManager();
        qDebug() << "NetworkManager 创建成功";

        // 创建 MessageDispatcher
        MessageDispatcher *messageDispatcher = new MessageDispatcher();
        qDebug() << "MessageDispatcher 创建成功";

        // 获取 UserSession 实例
        UserSession *userSession = UserSession::instance();
        qDebug() << "UserSession 实例获取成功:" << (userSession != nullptr);

        // 获取各个功能模块实例
        WordSearch *wordSearch = WordSearch::instance();
        WordCollect *wordCollect = WordCollect::instance();
        WordVocabulary *wordVocabulary = WordVocabulary::instance();
        qDebug() << "功能模块实例获取成功";

        // 设置 MessageDispatcher
        messageDispatcher->setNetworkManager(networkManager);
        messageDispatcher->setUserSession(userSession);
        messageDispatcher->setWordSearch(wordSearch);
        messageDispatcher->setWordCollect(wordCollect);
        messageDispatcher->setWordVocabulary(wordVocabulary);
        qDebug() << "MessageDispatcher 配置完成";

        userSession->setMessageDispatcher(messageDispatcher);
        wordSearch->setMessageDispatcher(messageDispatcher);
        wordCollect->setMessageDispatcher(messageDispatcher);
        wordVocabulary->setMessageDispatcher(messageDispatcher);
        qDebug() << "各模块 MessageDispatcher 设置完成";

        // 初始化 UserSession
        userSession->initialize();
        qDebug() << "UserSession 初始化完成";

        // 启动消息分发器
        messageDispatcher->start();
        qDebug() << "MessageDispatcher 启动完成";

        // 创建 PageController
        PageController *pageController = new PageController(window);
        qDebug() << "PageController 创建成功";

        // 添加到主窗口
        window->mainLayout()->addWidget(pageController);
        qDebug() << "PageController 添加到窗口完成";

        qDebug() << "服务器地址: 192.168.23.132:8080";
        qDebug() << "模块初始化完成";

        // 连接服务器
        if (networkManager->isConnected()) {
            qDebug() << "已经连接到服务器";
        } else {
            qDebug() << "正在连接到服务器...";
            networkManager->connectToServer();
            // 等待建立连接 - 使用QTimer避免阻塞
            QTimer::singleShot(1000, [&]() {
                qDebug() << "连接等待完成";
            });
            QApplication::processEvents();
        }

        // 显示主窗口
        window->show();
        qDebug() << "主窗口显示成功";

        qDebug() << "应用启动完成，进入事件循环";

        int result = app.exec();

        qDebug() << "应用即将退出，退出码:" << result;

        // 清理资源
        if (pageController) {
            pageController->hide();
            delete pageController;
            qDebug() << "PageController 已清理";
        }

        if (userSession) {
            userSession->setMessageDispatcher(nullptr);
            //userSession->reset();
            qDebug() << "UserSession 已清理";
        }

        if (messageDispatcher) {
            messageDispatcher->stop();
            messageDispatcher->deleteLater();
            qDebug() << "MessageDispatcher 已清理";
        }

        if (networkManager && networkManager->isConnected()) {
            networkManager->disconnectFromServer();
            networkManager->deleteLater();
            qDebug() << "NetworkManager 已清理";
        }

        if (window) {
            window->hide();
            delete window;
            qDebug() << "主窗口已清理";
        }

        qDebug() << "资源清理完成";

        return result;

    } catch (const std::exception& e) {
        qCritical() << "捕获到异常:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "捕获到未知异常";
        return -1;
    }
}
