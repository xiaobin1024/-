#include"network_manager.h"
NetworkManager::NetworkManager(QObject* parent,const AppConfig& config):
    BaseManager(parent),
    m_socket(new QTcpSocket(this)),
    m_serverIP(config.serverIp),
    m_serverPort(config.serverPort),
    m_reconnectTimer(new QTimer(this)),
    m_reconnectInterval(config.reconnectInterval),
    m_maxReconnectAttempts(config.maxReconnectAttempts),
    m_currentReconnectAttempts(0),
    m_autoReconnect(true),
    m_isReconnecting(false)

{
    qDebug()<<"NetworkManager constructor";

    //设置定时器
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer,&QTimer::timeout,this,&NetworkManager::onReconnectTimeout);

    // 初始化子管理器
    m_connectionManager = new ConnectionManager(this);
    // 设置连接
    setupConnections();

}
 void NetworkManager::setupConnections()
{
     // 连接信号槽
     connect(m_connectionManager, &ConnectionManager::connectionEstablished,
             this, &NetworkManager::onConnectionEstablished);
     connect(m_connectionManager, &ConnectionManager::connectionLost,
             this, &NetworkManager::onConnectionLost);
     connect(m_connectionManager, &ConnectionManager::networkError,
             this,[this](QString error){onNetworkError(error);});
     connect(m_connectionManager,&ConnectionManager::dataReady,
             this,&NetworkManager::onSocketReadyRead);
}

bool NetworkManager::isConnected() const
{
     return m_connectionManager->isConnected();
}

void NetworkManager::connectToServer()
{
    m_connectionManager->connectToServer(m_serverIP, m_serverPort);
}

void NetworkManager::disconnectFromServer()
{
    qDebug()<<"Disconnnecting from server";
    //设置标志，防止重连
    bool wasAutoReconnect=m_autoReconnect;
    m_autoReconnect=false;
    //停止所有定时器
    stopReconnect();

    m_connectionManager->disconnectFromServer();

    //恢复自动重连设置
    m_autoReconnect=wasAutoReconnect;
    //清空消息队列
    {
        QMutexLocker locker(&m_queueMutex);
        m_messageQueue.clear();
    }
    qDebug()<< "Disconnected from server";
}
void NetworkManager::onConnectionEstablished()
{
    qDebug() << "Connected to server successfully";
    stopReconnect();

    // 连接建立时立即处理积压队列
    if(!m_messageQueue.isEmpty())
    processMessageQueue();

    emit connectionEstablished();
}
void NetworkManager::onConnectionLost()
{
    qDebug() << "Disconnected from server";
    emit connectionLost();

    // 自动重连
    if(m_autoReconnect)
    {
        startReconnect();
    }
}
void NetworkManager::onNetworkError(QString &error)
{
    qDebug() << "Network error occurred:" << error;
    emit networkError(error);

    // 根据错误类型判断是否需要重连
    bool shouldReconnect = m_connectionManager->shouldReconnectOnError();
    if(shouldReconnect && m_autoReconnect && !m_isReconnecting)
    {
        startReconnect();
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
    return m_connectionManager->sendMessage(message);
}
void NetworkManager::onSocketReadyRead()
{
    qDebug()<<"进入NetworkManager::onSocketReadyRead()";
    QVector<CoreMessage::Msg> messages=m_connectionManager->readMessages();
    for(const auto& message: messages)
    {
        qDebug()<<"进入handleIncomingMessage";
        handleIncomingMessage(message);
    }
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
        emit onNetworkError(error);
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
