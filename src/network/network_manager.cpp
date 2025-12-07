#include"network_manager.h"
NetworkManager::NetworkManager(QObject* parent,const AppConfig& config):
    BaseManager(parent),
    m_socket(new QTcpSocket(this)),
    m_serverIP(config.serverIp),
    m_serverPort(config.serverPort),
    m_heartbeatTimer(new QTimer(this)),
    m_heartbeatInterval(config.heartbeatInterval),
    m_reconnectTimer(new QTimer(this)),
    m_reconnectInterval(config.reconnectInterval),
    m_maxReconnectAttempts(config.maxReconnectAttempts),
    m_currentReconnectAttempts(0),
    m_autoReconnect(true),
    m_isReconnecting(false)

{
    qDebug()<<"NetworkManager constructor";

    //设置定时器
    m_heartbeatTimer->setSingleShot(false);
    m_reconnectTimer->setSingleShot(true);
    //连接信号槽
    connect(m_socket,&QTcpSocket::connected,this,&NetworkManager::onSocketConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&NetworkManager::onSocketDisconnected);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&NetworkManager::onSocketError);
    connect(m_socket,&QTcpSocket::readyRead,this,&NetworkManager::onSocketReadyRead);
    connect(m_heartbeatTimer,&QTimer::timeout,this,&NetworkManager::onHeartbeatTimeout);
    connect(m_reconnectTimer,&QTimer::timeout,this,&NetworkManager::onReconnectTimeout);

}
bool NetworkManager::isConnected() const
{
    return m_socket && m_socket->state()==QTcpSocket::ConnectedState;
}

void NetworkManager::connectToServer()
{
    if(isConnected())
    {
        qDebug()<<"Already connected to server";
        return;
    }

    if(m_serverIP.isEmpty() || m_serverPort<=0)
    {
        qDebug()<<"Invalid IP address or port";
        QString error="Invalid IP address or port";
        emit networkError(error);
        return;
    }
    m_lastServerIp = m_serverIP;
    m_lastServerPort=m_serverPort;
    qDebug()<<"Connecting to server："<<m_serverIP<<":"<<m_serverPort;
    m_socket->connectToHost(m_serverIP,m_serverPort);
}

void NetworkManager::disconnectFromServer()
{
    qDebug()<<"Disconnnecting from server";
    //设置标志，防止重连
    bool wasAutoReconnect=m_autoReconnect;
    m_autoReconnect=false;
    //停止所有定时器
    stopHeartbeat();
    stopReconnect();

    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
    }
    //恢复自动重连设置
    m_autoReconnect=wasAutoReconnect;
    //清空消息队列
    {
        QMutexLocker locker(&m_queueMutex);
        m_messageQueue.clear();
    }
    qDebug()<< "Disconnected from server";
}

void NetworkManager::onSocketConnected()
{
    qDebug()<<"Connected to server successfully";

    stopReconnect();
    m_currentReconnectAttempts = 0;
    m_isReconnecting = false;

    // 连接建立时立即处理积压队列
    processMessageQueue();
    //启动心跳
    startHeartbeat();
    emit connectionEstablished();
}

void NetworkManager::onSocketDisconnected()
{
    qDebug()<<"Disconnected from server";
    //关闭心跳
    stopHeartbeat();
    emit connectionLost();

    // 自动重连
    if (m_autoReconnect) {
        startReconnect();
    }
}

void NetworkManager::onSocketError()
{
    QString errorString=m_socket->errorString();
    qDebug()<<"Socket error："<<errorString;
    //关闭心跳
    stopHeartbeat();
    emit networkError(errorString);

    QAbstractSocket::SocketError socketError=m_socket->error();
    //根据错误类型分类处理
    //如果是连接被拒绝，远程关闭，网络错误等，尝试重连
    if(m_autoReconnect && !m_isReconnecting)
    {
        switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
        case QAbstractSocket::RemoteHostClosedError:
        case QAbstractSocket::SocketTimeoutError:
        case QAbstractSocket::NetworkError:
            qDebug()<<"Socket error triggers reconnect";
            startReconnect();
            break;
        case QAbstractSocket::HostNotFoundError:
            //主机未找到可能是IP错误，不重连
            break;
        default:
            //其他问题不重连
            break;
        }
    }



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
    qint64 byteWritten;
    //发送消息结构体
    //确保同一时刻发送一条消息，添加锁
    {
         QMutexLocker locker(&m_writeMutex);
        byteWritten=m_socket->write(reinterpret_cast<const char*>(&netMsg),sizeof(CoreMessage::Msg));
    }

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
    //qDebug() << "onSocketReadyRead in, bytes available:" << m_socket->bytesAvailable();
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
        // qDebug()<<"handleIncomingMessage in ";
         receivedMessageTypes.append(receivedMsg.type);
        //处理接收到的数据
        handleIncomingMessage(receivedMsg);
    }
     //qDebug()<<"onSocketReadyRead out";
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
            // 发送失败，放到队列前端
            m_messageQueue.prepend(message);
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

void NetworkManager::startHeartbeat()
{
    if(m_heartbeatInterval>0)
    {
        m_heartbeatTimer->start(m_heartbeatInterval);
        qDebug()<<"Heartbeat started,interval: "<<m_heartbeatInterval<<"ms";
    }
}

void NetworkManager::stopHeartbeat()
{
    if(m_heartbeatTimer->isActive())
    {
        m_heartbeatTimer->stop();
        qDebug()<<"Heartbeat stopped";
    }
}

void NetworkManager::onHeartbeatTimeout()
{
    if(!isConnected())
    {
        qDebug()<<"Cannot send heartbeat: not connected";
        stopHeartbeat();
        return;
    }
    //创建心跳包
    CoreMessage::Msg heartbeatMsg;
    heartbeatMsg.type=CoreMessage::MsgType::HEARTBEAT;
    CoreUtils::StringUtils::safeStringCopy(heartbeatMsg.name,QString("client"),sizeof(heartbeatMsg.name));
    CoreUtils::StringUtils::safeStringCopy(heartbeatMsg.text,QString("ping"),sizeof(heartbeatMsg.text));
    //发送心跳
    if(sendRawMessage(heartbeatMsg))
    {
        emit heartbeatSent();
        qDebug()<<"Heartbeat sent at"<<CoreUtils::TimeUtils::currentTimestamp();
    }else{
        qDebug()<<"Failed to send heartbeat";
    }
}

void NetworkManager::startReconnect()
{
    qDebug()<<"进入startReconnect()";
    if(m_isReconnecting || !m_autoReconnect) return;


    m_isReconnecting=true;
    m_currentReconnectAttempts=0;
    //第一次尝试重连
    m_reconnectTimer->start(0);
    qDebug()<<"Reconnect process started";
}

void NetworkManager::stopReconnect()
{
    if(m_reconnectTimer->isActive() || m_isReconnecting)
    {
        m_reconnectTimer->stop();
        m_isReconnecting=false;
        m_currentReconnectAttempts=0;
        qDebug()<<"Reconnect process stoppde";
    }
}

void NetworkManager::onReconnectTimeout()
{
    if(m_currentReconnectAttempts>=m_maxReconnectAttempts){
        qDebug()<<"Max reconnect attempts reached: "<<m_maxReconnectAttempts;
        stopReconnect();
        QString error="Maximun reconnection attempts reached";
        emit networkError(error);
        return;
    }
    m_currentReconnectAttempts++;
    //发出重连尝试信号
    emit reconnectAttempt(m_currentReconnectAttempts,m_maxReconnectAttempts);
    qDebug() << "Reconnect attempt" << m_currentReconnectAttempts
             << "/" << m_maxReconnectAttempts;
    //尝试重连
    if(!m_lastServerIp.isEmpty() && m_lastServerPort>0)
    {
        qDebug() << "Attempting to reconnect to" << m_lastServerIp << ":" << m_lastServerPort;
        m_socket->connectToHost(m_lastServerIp, m_lastServerPort);
    }else
    {
        qDebug() << "No previous connection info available for reconnection";
        stopReconnect();
    }
    //设置下一次重连
    if(m_currentReconnectAttempts<m_maxReconnectAttempts)
    {
        int interval = qMax(100, m_reconnectInterval); // 确保最小间隔为100ms
        m_reconnectTimer->start(interval);
        qDebug() << "Next reconnect in" << interval << "ms";
    }
}
