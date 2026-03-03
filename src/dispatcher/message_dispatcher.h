// message_dispatcher.h
#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
#include"network_manager.h"

// 前向声明
class UserSession;

// 集中式消息分发器
class MessageDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MessageDispatcher(QObject* parent = nullptr);

    // 设置各模块
    void setNetworkManager(NetworkManager* manager);
    void setUserSession(UserSession* session);

    // 启动/停止分发
    void start();
    void stop();
    bool isStarted() const;

signals:
    // 特定类型消息信号
    void loginResponseReceived(const QString& responseData);
    void registerResponseReceived(const QString& responseData);
    void logoutResponseReceived(const QString& responseData);
    void searchResponseReceived(const QString& responseData);
    void historyResponseReceived(const QString& responseData);
    void collectResponseReceived(const QString& responseData);
    void queryCollectResponseReceived(const QString& responseData);

    // 通用消息信号
    void messageDispatched(CoreMessage::MsgType type, const QString& data);

    // 状态信号
    void networkStateChanged(bool connected);
    void started();
    void stopped();
    void errorOccurred(const QString& message);

private slots:
    void onMessageReceived(const CoreMessage::Msg& message);
    void onLoginRequested(const QString& username, const QString& password);
    void onLogoutRequested();
    void onRegisterRequested(const QString& username, const QString& password);

    // 网络状态处理
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkError(const QString& error);

private:
    void dispatchMessage(const CoreMessage::Msg& message);
    void connectNetworkSignals();
    void disconnectNetworkSignals();

    // 发送消息辅助函数
    bool sendMessage(CoreMessage::MsgType type, const QString& name,
                     const QString& text = "");

    NetworkManager* m_networkManager = nullptr;
    UserSession* m_userSession = nullptr;
    bool m_started = false;
    bool m_networkConnected = false;
};

#endif // MESSAGE_DISPATCHER_H
