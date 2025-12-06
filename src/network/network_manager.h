#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include"core/base_manager.h"
#include<QTcpSocket>
#include<QTimer>
#include<QQueue>
#include <QMutex>

class NetworkManager: public BaseManager
{
    Q_OBJECT
public:

    explicit NetworkManager(QObject* parent=nullptr);
    //实现BaseManager接口
    bool isConnected() const override;
    void connectToServer(const QString &ip,int port) override;
    void disconnectFromServer() override;
    bool sendMessage(const CoreMessage::Msg &message) override;

    //发送消息接口
    bool sendRawMessage(const CoreMessage::Msg &message);
    //处理接收到的数据接口
    void handleIncomingMessage(const CoreMessage::Msg &message);
    //异步发送消息
    void sendMessageAsync(const CoreMessage::Msg &message);
    //获取待处理的消息数量
    int getPendingMessageCount();

signals:
    void networkError(QString &error);
    void connectionEstablished();
    void connectionLost();
    void messageReceived(const  CoreMessage::Msg &message); //向上层传递收到数据信号
private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError();
    void onSocketReadyRead();    //接收消息接口
    void processMessageQueue(); //处理消息队列

private:
    //消息队列相关变量
    QQueue<CoreMessage::Msg> m_messageQueue;    //消息队列
    QMutex m_queueMutex;    //互斥锁保护消息队列

    QTcpSocket *m_socket;
    QTimer *m_heartbeatTimer;
    QTimer *m_reconnectTimer;

};

#endif // NETWORK_MANAGER_H
