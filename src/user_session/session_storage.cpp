#include "session_storage.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

SessionStorage::SessionStorage(QObject* parent)
    : QObject(parent)
{
    // 设置默认存储路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_storagePath = appDataPath + "/data";

    // 确保目录存在
    ensureDirectoryExists(m_storagePath);
}

void SessionStorage::setStoragePath(const QString& path)
{
    if (m_storagePath != path) {
        m_storagePath = path;
        ensureDirectoryExists(m_storagePath);
    }
}

void SessionStorage::setFileName(const QString& name)
{
    if (m_fileName != name) {
        m_fileName = name;
    }
}

QString SessionStorage::storagePath() const
{
    return m_storagePath;
}

QString SessionStorage::fileName() const
{
    return m_fileName;
}

bool SessionStorage::saveSession(const UserData& userData)
{
    if (!userData.isValid()) {
        emit error("无法保存会话: 用户数据无效");
        return false;
    }

    QString filePath = getSessionFilePath();
    QJsonObject json = userData.toJson();

    if (writeJsonFile(filePath, json)) {
        emit sessionSaved(userData.userId());
        qDebug() << "会话已保存:" << filePath;
        return true;
    } else {
        emit error("保存会话失败: " + filePath);
        return false;
    }
}

UserData SessionStorage::loadSession()
{
    QString filePath = getSessionFilePath();
    QJsonObject json = readJsonFile(filePath);

    if (json.isEmpty()) {
        return UserData();  // 返回空的UserData
    }

    UserData userData = UserData::fromJson(json);

    if (userData.isValid()) {
        emit sessionLoaded(userData);
        qDebug() << "会话已加载:" << userData.username();
    }

    return userData;
}

bool SessionStorage::clearSession()
{
    QString filePath = getSessionFilePath();
    QFile file(filePath);

    if (file.exists()) {
        if (file.remove()) {
            emit sessionCleared();
            qDebug() << "会话已清除:" << filePath;
            return true;
        } else {
            emit error("清除会话失败: " + file.errorString());
            return false;
        }
    }

    return true; // 文件不存在也算清除成功
}

bool SessionStorage::hasStoredSession() const
{
    QString filePath = getSessionFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        return false;
    }

    // 检查文件是否有有效内容
    QJsonObject json = readJsonFile(filePath);
    UserData userData = UserData::fromJson(json);
    return userData.isValid();
}

bool SessionStorage::saveUserData(const QString& userId, const QJsonObject& data)
{
    if (userId.isEmpty()) {
        emit error("无法保存用户数据: 用户ID为空");
        return false;
    }

    QString filePath = getUserDataFilePath(userId);

    if (writeJsonFile(filePath, data)) {
        qDebug() << "用户数据已保存:" << userId << "到" << filePath;
        return true;
    } else {
        emit error("保存用户数据失败: " + filePath);
        return false;
    }
}

QJsonObject SessionStorage::loadUserData(const QString& userId)
{
    if (userId.isEmpty()) {
        return QJsonObject();
    }

    QString filePath = getUserDataFilePath(userId);
    QJsonObject json = readJsonFile(filePath);

    if (json.isEmpty()) {
        qDebug() << "用户数据文件为空或不存在:" << filePath;
    } else {
        qDebug() << "用户数据已加载:" << userId;
    }

    return json;
}

QString SessionStorage::getSessionFilePath() const
{
    return m_storagePath + "/" + m_fileName;
}

QString SessionStorage::getUserDataFilePath(const QString& userId) const
{
    // 使用用户ID作为文件名的一部分
    QString safeUserId = userId;
    safeUserId.replace("/", "_").replace("\\", "_").replace(":", "_");
    return m_storagePath + "/user_" + safeUserId + ".json";
}

bool SessionStorage::ensureDirectoryExists(const QString& path) const
{
    QDir dir(path);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "无法创建目录: " + path;
            return false;
        }
    }
    return true;
}

bool SessionStorage::writeJsonFile(const QString& filePath, const QJsonObject& json) const
{
    // 确保目录存在
    QFileInfo fileInfo(filePath);
    if (!ensureDirectoryExists(fileInfo.absolutePath())) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件写入:" << filePath << file.errorString();
        return false;
    }

    QJsonDocument doc(json);
    qint64 bytesWritten = file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    if (bytesWritten == -1) {
        qWarning() << "写入文件失败:" << filePath;
        return false;
    }

    return true;
}

QJsonObject SessionStorage::readJsonFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        return QJsonObject();
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString() << "文件:" << filePath;
        return QJsonObject();
    }

    if (!doc.isObject()) {
        qWarning() << "JSON文档不是对象:" << filePath;
        return QJsonObject();
    }

    return doc.object();
}
