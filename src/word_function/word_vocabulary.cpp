#include "word_vocabulary.h"
#include "message_dispatcher.h"
#include <QStringList>
#include <QDebug>

WordVocabulary* WordVocabulary::s_instance = nullptr;

WordVocabulary* WordVocabulary::instance()
{
    if (!s_instance) {
        s_instance = new WordVocabulary();
    }
    return s_instance;
}

WordVocabulary::WordVocabulary(QObject* parent)
    : QObject(parent)
{
}

WordVocabulary::~WordVocabulary()
{
    s_instance = nullptr;
}

void WordVocabulary::setMessageDispatcher(MessageDispatcher* dispatcher)
{
    if (m_dispatcher == dispatcher) {
        return;
    }

    // 断开旧的连接
    if (m_dispatcher) {
        disconnect(m_dispatcher, &MessageDispatcher::vocabularyResponseReceived,
                   this, &WordVocabulary::handleVocabularyResponse);
    }

    m_dispatcher = dispatcher;

    // 连接新的信号
    if (m_dispatcher) {
        connect(m_dispatcher, &MessageDispatcher::vocabularyResponseReceived,
                this, &WordVocabulary::handleVocabularyResponse);

        qDebug() << "MessageDispatcher 已设置到 WordVocabulary";
    }
}

void WordVocabulary::setUserSession(UserSession* session)
{
    m_userSession = session;
}

void WordVocabulary::updateVocabularyStatus(const WordData& wordData)
{
    if (!m_userSession) {
        emit vocabularyOperationFailed("用户会话未设置");
        return;
    }

    if (!wordData.isValid()) {
        emit vocabularyOperationFailed("单词数据无效");
        return;
    }

    // 获取当前用户名
    QString username = m_userSession->currentUser().username();
    if (username.isEmpty()) {
        emit vocabularyOperationFailed("用户未登录");
        return;
    }

    // 构建生词本数据字符串
    // 格式: 单词|^|释义|^|音标|^|例句|^|翻译|^|当前生词本状态
    QString vocabularyData = QString("%1|^|%2|^|%3|^|%4|^|%5|^|%6")
                                 .arg(wordData.word)
                                 .arg(wordData.meaning)
                                 .arg(wordData.phonetic)
                                 .arg(wordData.example)
                                 .arg(wordData.translation)
                                 .arg(wordData.isVocabulary ? "1" : "0");

    qDebug() << "WordVocabulary::updateVocabularyStatus - 发送生词本请求:" << vocabularyData;

    // 发射生词本操作请求信号
    emit vocabularyStatusRequested(username, vocabularyData);
}

void WordVocabulary::handleVocabularyResponse(const QString& responseData)
{
    processVocabularyResponse(responseData);
}

void WordVocabulary::processVocabularyResponse(const QString& response)
{
    qDebug() << "WordVocabulary::processVocabularyResponse - 响应:" << response;

    if (response.startsWith("ERROR:")) {
        emit vocabularyOperationFailed(response);
        return;
    }

    // --- 处理后端返回的响应码 ---
    if (response == "add_success") {
        emit vocabularyStatusChanged(true); // 发射状态改变信号，现在在生词本中
        emit vocabularyOperationSuccess("添加生词成功");
        return;
    }

    if (response == "add_error") {
        emit vocabularyOperationFailed("添加生词失败");
        return;
    }

    if (response == "remove_success") {
        emit vocabularyStatusChanged(false); // 发射状态改变信号，现在不在生词本中
        emit vocabularyOperationSuccess("移出生词本成功");
        return;
    }

    if (response == "remove_error") {
        emit vocabularyOperationFailed("移出生词本失败");
        return;
    }

    if (response == "format is incorrect") {
        emit vocabularyOperationFailed("数据格式错误");
        return;
    }

    // 默认处理
    emit vocabularyOperationFailed("操作失败，未知响应: " + response);
}
