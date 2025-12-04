#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include"core/base_manager.h"
#include<QTcpSocket>
#include<QTimer>
#include<QQueue>

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

signals:
    void networkError(QString &error);
    void connectionEstablished();
    void connectionLost();
private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError();
private:
    QTcpSocket *m_socket;
    QTimer *m_heartbeatTimer;
    QTimer *m_reconnectTimer;

};

#endif // NETWORK_MANAGER_H
