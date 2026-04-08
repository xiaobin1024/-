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
     m_userId = "";
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

void SessionStorage::setUserId(const QString& userId)
{
    if (m_userId != userId) {
        m_userId = userId;
    }
}

// void SessionStorage::setFileName(const QString& name)
// {
//     if (m_fileName != name) {
//         m_fileName = name;
//     }
// }

QString SessionStorage::storagePath() const
{
    return m_storagePath;
}

QString SessionStorage::userId() const
{
    return m_userId;
}

// QString SessionStorage::fileName() const
// {
//     return m_fileName;
// }

// bool SessionStorage::saveSession(const UserData& userData)
// {
//     if (!userData.isValid()) {
//         emit error("无法保存会话: 用户数据无效");
//         return false;
//     }

//     QString filePath = getSessionFilePath();
//     QJsonObject json = userData.toJson();

//     if (writeJsonFile(filePath, json)) {
//         emit sessionSaved(userData.userId());
//         qDebug() << "会话已保存:" << filePath;
//         return true;
//     } else {
//         emit error("保存会话失败: " + filePath);
//         return false;
//     }
// }

// UserData SessionStorage::loadSession()
// {
//     QString filePath = getSessionFilePath();
//     QJsonObject json = readJsonFile(filePath);

//     if (json.isEmpty()) {
//         return UserData();  // 返回空的UserData
//     }

//     UserData userData = UserData::fromJson(json);

//     if (userData.isValid()) {
//         emit sessionLoaded(userData);
//         qDebug() << "会话已加载:" << userData.username();
//     }

//     return userData;
// }

// bool SessionStorage::clearSession()
// {
//     QString filePath = getSessionFilePath();
//     QFile file(filePath);

//     if (file.exists()) {
//         if (file.remove()) {
//             emit sessionCleared();
//             qDebug() << "会话已清除:" << filePath;
//             return true;
//         } else {
//             emit error("清除会话失败: " + file.errorString());
//             return false;
//         }
//     }

//     return true; // 文件不存在也算清除成功
// }

// bool SessionStorage::hasStoredSession() const
// {
//     QString filePath = getSessionFilePath();
//     QFile file(filePath);

//     if (!file.exists()) {
//         return false;
//     }

//     // 检查文件是否有有效内容
//     QJsonObject json = readJsonFile(filePath);
//     UserData userData = UserData::fromJson(json);
//     return userData.isValid();
// }


bool SessionStorage::saveUserData(const QJsonObject& userData)
{
    if (m_userId.isEmpty()) {
        emit error("无法保存用户数据: 用户ID为空");
        return false;
    }

    QString filePath = getUserDataFilePath();
    QJsonObject json = userData;

    if (writeJsonFile(filePath, json)) {
        emit userDataSaved();
        qDebug() << "用户数据已保存:" << filePath;
        return true;
    } else {
        emit error("保存用户数据失败: " + filePath);
        return false;
    }
}

QJsonObject SessionStorage::loadUserData() const
{
    QString filePath = getUserDataFilePath();
    QJsonObject json = readJsonFile(filePath);

    if (json.isEmpty()) {
        qDebug() << "用户数据文件为空或不存在:" << filePath;
    } else {
        qDebug() << "用户数据已加载:" << m_userId<<"文件路径："<<filePath;
    }

    return json;
}


QString SessionStorage::getSessionFilePath() const
{
    return m_storagePath + "/" + m_fileName;
}

QString SessionStorage::getUserDataFilePath() const
{
    // 当m_userId为空时，返回一个默认路径或查找最近的文件
    if (m_userId.isEmpty()) {
        qWarning() << "警告：用户ID为空，无法构建文件路径";
        return "";
    }

    // 正常情况下的路径生成
    QString safeUserId = m_userId;
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
    // 记录函数开始执行
    qDebug() << "开始读取JSON文件:" << filePath;

    QFile file(filePath);

    // 检查文件是否存在
    if (!file.exists()) {
        qWarning() << "文件不存在:" << filePath;
        return QJsonObject();
    }

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filePath << "错误:" << file.errorString();
        return QJsonObject();
    }

    // 读取文件内容
    QByteArray data = file.readAll();
    file.close();

    // 记录读取的数据大小
    qDebug() << "成功读取文件:" << filePath << "数据大小:" << data.size() << "字节";

    // 检查数据是否为空
    if (data.isEmpty()) {
        qWarning() << "文件内容为空:" << filePath;
        return QJsonObject();
    }

    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    // 检查解析错误
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString() << "文件:" << filePath;
        return QJsonObject();
    }

    // 检查文档类型
    if (!doc.isObject()) {
        qWarning() << "JSON文档不是对象:" << filePath;
        return QJsonObject();
    }

    // 记录解析成功
    qDebug() << "成功解析JSON文件:" << filePath;

    return doc.object();
}

bool SessionStorage::deleteUserData()
{
    if (m_userId.isEmpty()) {
        emit error("无法清除用户数据: 用户ID为空");
        return false;
    }

    QString filePath = getUserDataFilePath();
    QFile file(filePath);

    if (file.exists()) {
        if (file.remove()) {
            emit userDataCleared();
            qDebug() << "用户数据已清除:" << filePath;
            return true;
        } else {
            emit error("清除用户数据失败: " + file.errorString());
            return false;
        }
    }

    return true; // 文件不存在也算清除成功
}

// bool SessionStorage::hasStoredUserData() const
// {
//    return !findLastModifiedUserFile().isEmpty();
// }

QString SessionStorage::findLastModifiedUserFile() const
{
    QDir dir(m_storagePath);
    if (!dir.exists()) {
        return "";
    }

    // 查找所有以"user_"开头的JSON文件
    QStringList filters;
    filters << "user_*.json";
    dir.setNameFilters(filters);

    // 按修改时间排序，获取最新文件
    QFileInfoList files = dir.entryInfoList();
    if (files.isEmpty()) {
        return "";
    }

    // 排序文件，获取最新修改的
    std::sort(files.begin(), files.end(), [](const QFileInfo& a, const QFileInfo& b) {
        return a.lastModified() > b.lastModified();
    });

    return files.first().absoluteFilePath();
}

bool SessionStorage::hasAnyStoredUserData() const
{
    QDir dir(m_storagePath);
    if (!dir.exists()) {
        return false;
    }

    // 查找所有以"user_"开头的JSON文件
    QStringList filters;
    filters << "user_*.json";
    dir.setNameFilters(filters);

    return !dir.entryList().isEmpty();
}
