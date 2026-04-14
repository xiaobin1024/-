#include "word_collect.h"
#include "message_dispatcher.h"
#include <QStringList>
#include <QDebug>

WordCollect* WordCollect::s_instance = nullptr;

WordCollect* WordCollect::instance()
{
    if (!s_instance) {
        s_instance = new WordCollect();
    }
    return s_instance;
}

WordCollect::WordCollect(QObject* parent)
    : QObject(parent)
{
}

WordCollect::~WordCollect()
{
    s_instance = nullptr;
}

void WordCollect::setMessageDispatcher(MessageDispatcher* dispatcher)
{
    if (m_dispatcher == dispatcher) {
        return;
    }

    // 断开旧的连接
    if (m_dispatcher) {
        disconnect(m_dispatcher, &MessageDispatcher::collectResponseReceived,
                   this, &WordCollect::handleCollectResponse);
        disconnect(m_dispatcher, &MessageDispatcher::CollectResponseListReceived,
                   this, &WordCollect::handleCollectListResponse);
    }

    m_dispatcher = dispatcher;

    // 连接新的信号
    if (m_dispatcher) {
        connect(m_dispatcher, &MessageDispatcher::collectResponseReceived,
                this, &WordCollect::handleCollectResponse);
        connect(m_dispatcher, &MessageDispatcher::CollectResponseListReceived,
                this, &WordCollect::handleCollectListResponse);

        qDebug() << "MessageDispatcher 已设置到 WordCollect";
    }
}

void WordCollect::setUserSession(UserSession* session)
{
    m_userSession = session;
}

void WordCollect::collectWord(const WordData& wordData)
{
    if (!m_userSession) {
        emit collectFailed("用户会话未设置");
        return;
    }

    if (!wordData.isValid()) {
        emit collectFailed("单词数据无效");
        return;
    }

    // 获取当前用户名
    QString username = m_userSession->currentUser().username();
    if (username.isEmpty()) {
        emit collectFailed("用户未登录");
        return;
    }

    // 构建收藏数据字符串
    // 格式: 单词|^|释义|^|音标|^|例句|^|翻译|^|收藏状态
    QString collectData = QString("%1|^|%2|^|%3|^|%4|^|%5|^|%6")
                              .arg(wordData.word)
                              .arg(wordData.meaning)
                              .arg(wordData.phonetic)
                              .arg(wordData.example)
                              .arg(wordData.translation)
                              .arg(wordData.isCollected ? "1" : "0");

    qDebug() << "WordCollect::collectWord - 发送收藏请求:" << collectData;

    // 发射收藏请求信号
    emit collectRequested(username, collectData);
}

void WordCollect::queryCollectList()
{
    if (!m_userSession) {
        emit collectListFailed("用户会话未设置");
        return;
    }

    // 获取当前用户名
    QString username = m_userSession->currentUser().username();
    if (username.isEmpty()) {
        emit collectListFailed("用户未登录");
        return;
    }

    qDebug() << "WordCollect::queryCollectList - 查询收藏列表，用户:" << username;

    // 发射查询收藏列表请求信号
    emit collectListRequested(username);
}

void WordCollect::handleCollectResponse(const QString& responseData)
{
    processCollectResponse(responseData);
}

void WordCollect::handleCollectListResponse(const QString& responseData)
{
    processCollectListResponse(responseData);
}

// word_collect.cpp 或相关文件

void WordCollect::processCollectResponse(const QString& response)
{
    qDebug() << "WordCollect::processCollectResponse - 响应:" << response;

    if (response.startsWith("ERROR:")) {
        emit collectFailed(response);
        return;
    }

    // --- 新的响应码处理 ---
    if (response == "collect_success") {
        emit collectStatusChanged(true); // 发射收藏状态改变信号，现在是收藏状态
        emit collectSuccess("收藏成功");
        return;
    }

    if (response == "collect_error") {
        // emit collectStatusChanged(false); // 状态未变，不发射
        emit collectFailed("收藏失败");
        return;
    }

    if (response == "cancel_success") {
        emit collectStatusChanged(false); // 发射收藏状态改变信号，现在是未收藏状态
        emit collectSuccess("取消收藏成功");
        return;
    }

    if (response == "cancel_error") {
        // emit collectStatusChanged(true); // 状态未变，不发射
        emit collectFailed("取消收藏失败");
        return;
    }

    if (response == "cancel_not_found") {
        // 这种情况下，服务器认为该单词未被收藏，客户端也应该同步状态
        emit collectStatusChanged(false); // 发射收藏状态改变信号，同步为未收藏状态
        emit collectSuccess("收藏已取消"); // 可以提示用户"已取消收藏"或"收藏不存在"
        return;
    }

    if (response == "format is incorrect") {
        emit collectFailed("数据格式错误");
        return;
    }

    if (response == "unknown error") {
        emit collectFailed("未知错误");
        return;
    }

    // 默认处理
    emit collectFailed("操作失败，未知响应: " + response);
}

void WordCollect::processCollectListResponse(const QString& response)
{
    qDebug() << "WordCollect::processCollectListResponse - 响应:" << response;

    if (response.startsWith("ERROR:")) {
        emit collectListFailed(response);
        return;
    }

    if (response == "empty") {
        m_collectList.clear();
        emit collectListSuccess(m_collectList);
        return;
    }
    if(response=="**OVER**"){
        emit collectListFailed(response);
        return;
    }
    m_collectList.clear();

    // 后端格式：单词|^|释义|^|音标|^|例句|^|翻译
    QStringList lines = response.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QStringList parts = line.split("|^|");

        // 检查是否有 5 个字段
        if (parts.size() >= 5) {
            WordData wordData;

            // 按顺序赋值
            wordData.word = parts[0];
            wordData.meaning = parts[1];
            wordData.phonetic = parts[2];
            wordData.example = parts[3];
            wordData.translation = parts[4];
            wordData.isCollected = true;

            if (wordData.isValid()) {
                m_collectList.append(wordData);
            }
        } else {
            qDebug() << "数据格式错误，字段不足:" << line;
        }
    }

    if (m_collectList.isEmpty()) {
        emit collectListFailed("没有收藏记录");
    } else {
        emit collectListSuccess(m_collectList);
    }
}
