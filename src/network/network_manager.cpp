#include"network_manager.h"
NetworkManager::NetworkManager(QObject* parent):BaseManager(parent),m_socket(new QTcpSocket(this))
{
    qDebug()<<"NetworkManager constructor";
    //连接信号槽
    connect(m_socket,&QTcpSocket::connected,this,&NetworkManager::onSocketConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&NetworkManager::onSocketDisconnected);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&NetworkManager::onSocketError);

}
bool NetworkManager::isConnected() const
{
    return m_socket && m_socket->state()==QTcpSocket::ConnectedState;
}

void NetworkManager::connectToServer(const QString &ip,int port)
{
    if(isConnected())
    {
        qDebug()<<"Already connected to server";
        return;
    }
    if(ip.isEmpty() || port<=0)
    {
        qDebug()<<"Invalid IP address or port";
        QString error="Invalid IP address or port";
        emit networkError(error);
        return;
    }
    m_serverIp=ip;
    m_serverPort=port;
    qDebug()<<"Connecting to server："<<ip<<":"<<port;
    m_socket->connectToHost(ip,port);
}

void NetworkManager::disconnectFromServer()
{
    qDebug()<<"Disconnnecting from server";
    if(m_socket)
    {
        m_socket->disconnectFromHost();
        if(m_socket->state()!=QAbstractSocket::UnconnectedState)
        {
            m_socket->waitForDisconnected(3000);
        }
    }
}

void NetworkManager::onSocketConnected()
{
    qDebug()<<"Connected to server successfully";
    emit connectionEstablished();
}

void NetworkManager::onSocketDisconnected()
{
    qDebug()<<"Disconnected from server";
    emit connectionLost();
}

void NetworkManager::onSocketError()
{
    QString errorString=m_socket->errorString();
    qDebug()<<"Socket error："<<errorString;
    emit networkError(errorString);
}

bool NetworkManager::sendMessage(const CoreMessage::Msg &message)
{
    if(!isConnected())
    {
        qDebug()<<"Cannot send message: not connected to server";
        return false;
    }
    //验证消息
    if(strlen(message.name)>=sizeof(message.name) ||
        strlen(message.text)>=sizeof(message.text))
    {
        qDebug()<<"Cannot send message: name or text too long";
        return false;
    }
    return sendRawMessage(message);
}
bool NetworkManager::sendRawMessage(const CoreMessage::Msg &message)
{
    if(!isConnected())
    {
        qDebug()<<"Cannot send raw message: not connected to server";
        return false;
    }
    //创建消息，并转换为网络字节序
    CoreMessage::Msg netMsg = message;
    netMsg.toNetWorkByteOrder();
    //发送消息结构体
    qint64 byteWritten=m_socket->write(reinterpret_cast<const char*>(&netMsg),sizeof(CoreMessage::Msg));
    if(byteWritten==-1)
    {
        qDebug()<<"Failed to write data  to socket: "<<m_socket->errorString();
        return false;
    }
    if(byteWritten!=sizeof(CoreMessage::Msg))
    {
        qDebug() << "Incomplete message sent:" << byteWritten << "of" << sizeof(CoreMessage::Msg) << "bytes";
        return false;
    }
    //等待数据写入完成
    if(!m_socket->waitForBytesWritten(3000))
    {
        qDebug() << "Timeout waiting for bytes to be written";
        return false;
    }
    qDebug() << "Message sent successfully, type:" << message.type
             << ", size:" << byteWritten << "bytes";
    return true;
}
