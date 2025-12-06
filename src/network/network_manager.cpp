#include"network_manager.h"
NetworkManager::NetworkManager(QObject* parent):BaseManager(parent),m_socket(new QTcpSocket(this))
{
    qDebug()<<"NetworkManager constructor";
    //连接信号槽
    connect(m_socket,&QTcpSocket::connected,this,&NetworkManager::onSocketConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&NetworkManager::onSocketDisconnected);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&NetworkManager::onSocketError);
    connect(m_socket,&QTcpSocket::readyRead,this,&NetworkManager::onSocketReadyRead);

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
    // 连接建立时立即处理积压队列
    processMessageQueue();
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
void NetworkManager::onSocketReadyRead()
{
    qDebug() << "onSocketReadyRead in, bytes available:" << m_socket->bytesAvailable();
    if(!isConnected())
    {
        qDebug()<<"Cannot send raw message: not connected to server";
        return ;
    }
    //持续读取直到没有完整消息
    while(m_socket->bytesAvailable()>=static_cast<qint64>(sizeof(CoreMessage::Msg)))
    {
        //直接读取完整消息结构体
        CoreMessage::Msg receivedMsg;
        qint64 bytesRead=m_socket->read(reinterpret_cast<char*>(&receivedMsg),sizeof(CoreMessage::Msg));
        if(bytesRead!=sizeof(CoreMessage::Msg))
        {
            qDebug()<<"Incomplete message received: "<<bytesRead<<"of"<<sizeof(CoreMessage::Msg)<<"bytes";
            break;
        }
        //转换字节序
        receivedMsg.toHostByteOrder();
         qDebug()<<"handleIncomingMessage in ";
        //处理接收到的数据
        handleIncomingMessage(receivedMsg);
    }
     qDebug()<<"onSocketReadyRead out";
}
void NetworkManager::handleIncomingMessage(const CoreMessage::Msg &message)
{
    qDebug()<<"Received message: "<<message;
    //根据消息类型处理
    switch(message.type)
    {
    case CoreMessage::MsgType::HEARTBEAT:
        qDebug()<<"Heartbeat received from: "<<message.name;
        break;
    case CoreMessage::MsgType::LOGIN:
        qDebug()<<"Login request from: "<<message.name;
        break;
    case CoreMessage::MsgType::REGISTER:
        qDebug()<<"Register request from: "<<message.name;
        break;
    case CoreMessage::MsgType::SEARCH:
        qDebug()<<"Search request: "<<message.text;
        break;
    case CoreMessage::MsgType::HISTORY:
        qDebug() << "History query from:" << message.name;
        break;

    case CoreMessage::MsgType::COLLECT:
        qDebug() << "Collect request:" << message.text;
        break;

    case CoreMessage::MsgType::QUERYCOLLECT:
        qDebug() << "Query collect from:" << message.name;
        break;

    case CoreMessage::MsgType::Quit:
        qDebug() << "Quit request from:" << message.name;
        break;

    default:
        qDebug() << "Unknown message type received:" << static_cast<int>(message.type);
        break;
    }
    emit messageReceived(message);
}

void NetworkManager::sendMessageAsync(const CoreMessage::Msg &message)
{
    QMutexLocker locker(&m_queueMutex);

    if(!isConnected())
    {
        // 连接断开时加入队列
        m_messageQueue.enqueue(message);
        qDebug() << "Message queued, pending count:" << m_messageQueue.size();
    }
    else
    {
        if(!m_messageQueue.isEmpty()){
            // 队列未清空，新消息入队
            m_messageQueue.enqueue(message);
            qDebug() << "Queue not empty, new message queued, pending count:" << m_messageQueue.size();
            // 连接正常时先尝试处理队列
            processMessageQueue();
        }else{
            if(!sendRawMessage(message)) {
                // 发送失败加入队列
                m_messageQueue.enqueue(message);
                qDebug() << "Message send failed, queued, pending count:" << m_messageQueue.size();
            }
        }
    }
}

void NetworkManager::processMessageQueue()
{
    QMutexLocker locker(&m_queueMutex);
    int processedCount = 0;
    int failedCount = 0;

    while(!m_messageQueue.isEmpty()) {
        CoreMessage::Msg message = m_messageQueue.dequeue(); // 直接出队尝试发送

        if(sendRawMessage(message)) {
            processedCount++;
        } else {
            // 发送失败重新入队
            m_messageQueue.prepend(message); // 放回队列前端
            failedCount++;
            break;
        }
    }

    if(processedCount > 0 || failedCount > 0) {
        qDebug() << "Message queue processed:" << processedCount << "sent,"
                 << failedCount << "failed, remaining:" << m_messageQueue.size();
    }
}
 int NetworkManager::getPendingMessageCount()
{
    QMutexLocker locker(&m_queueMutex);
    return m_messageQueue.size();
}
