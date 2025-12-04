#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H
#include"message.h"
#include<QObject>
    class BaseManager:public QObject
    {
        Q_OBJECT
    public:
        explicit BaseManager(QObject *parent=nullptr):QObject(parent){}
        virtual ~BaseManager()=default;

        //连接管理
        virtual bool isConnected() const=0;
        virtual void connectToServer(const QString &ip,int port)=0;
        virtual void disconnectFromServer()=0;

        //消息发送接口
        virtual bool sendMessage(const CoreMessage::Msg& message)=0;
    signals:
        void connectionStateChanged(bool connected);
        void messageReceived(const CoreMessage::Msg& message);
        void errorOccurred(const QString& error);
    protected:
        QString m_serverIp;
        int m_serverPort{0};
        bool m_isConnected{false};
    };


#endif // BASE_MANAGER_H
