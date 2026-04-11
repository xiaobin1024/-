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

QString SessionStorage::storagePath() const
{
    return m_storagePath;
}

QString SessionStorage::userId() const
{
    return m_userId;
}


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



QString SessionStorage::getUserDataFilePath() const
{
    // 当m_userId为空时，返回一个默认路径或查找最近的文件
    if (m_userId.isEmpty()) {
        qWarning() << "警告：用户ID为空，无法构建文件路径";
        return "";
    }

    //构建用户专属目录路径
    QString userDirPath=m_storagePath+"/"+m_userId;
    //确保目录存在（如果不存在，创建它）
    QDir dir;
    if(!dir.exists(userDirPath)){
        qWarning()<<"无法创建用户目录："<<userDirPath;
        return "";
    }

    //返回目录下的user.json路径
    return userDirPath+"/user.json";
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

bool SessionStorage::saveUserAvatar(const QString& imagePath)
{
    if (m_userId.isEmpty()) {
        emit error("无法保存头像: 用户ID为空");
        return false;
    }

    if (imagePath.isEmpty() || !QFile::exists(imagePath)) {
        emit error("无法保存头像: 源文件不存在");
        return false;
    }

    // 1. 确保用户数据目录存在
    QString userDir = m_storagePath + "/" + m_userId;
    if (!ensureDirectoryExists(userDir)) {
        return false;
    }

    // 2. 生成目标文件名 (例如: avatar.jpg)
    QString fileExtension = QFileInfo(imagePath).suffix();
    QString targetFileName = "avatar." + fileExtension;
    QString targetFilePath = userDir + "/" + targetFileName;

    // 3. 复制文件到目标位置
    if (QFile::exists(targetFilePath)) {
        QFile::remove(targetFilePath); // 如果已存在则覆盖
    }
    if (!QFile::copy(imagePath, targetFilePath)) {
        emit error("无法保存头像: 复制文件失败");
        return false;
    }

    // 4. 更新 JSON 数据中的 avatar 字段
    // 先加载现有数据
    QJsonObject userData = loadUserData();
    if (userData.isEmpty()) {
        // 如果没有旧数据，创建一个新的
        userData["userId"] = m_userId;
        userData["loggedIn"] = false;
    }

    // 相对路径存储建议： "users/user_123/avatar.jpg"
    // 这里我们存储相对于 data 目录的路径，方便后续加载
    QString relativePath = "data/" + m_userId + "/" + targetFileName;
    userData["avatar"] = relativePath;

    // 5. 保存回 JSON 文件
    return saveUserData(userData);
}

QString SessionStorage::findLatestUserDirectory() const
{
    QDir dir(m_storagePath);
    if (!dir.exists()) {
        return "";
    }

    // 1. 只查找目录，不查找文件
    // 如果你想限制文件夹名为纯数字，可以使用 setNameFilters({"[0-9]*"})，但通常 Qt 不支持正则过滤，这里简单查找所有目录
    QFileInfoList folders = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (folders.isEmpty()) {
        return "";
    }

    // 2. 遍历目录，检查里面是否有 user.json，并找出最新的一个
    QFileInfo latestFolderInfo;
    QDateTime latestTime;

    for (const QFileInfo& folderInfo : folders) {
        QString folderPath = folderInfo.absoluteFilePath();
        QString jsonPath = folderPath + "/user.json";

        // 检查该目录下是否存在 user.json
        if (QFile::exists(jsonPath)) {
            QFileInfo jsonFileInfo(jsonPath);
            // 比较修改时间
            if (jsonFileInfo.lastModified() > latestTime) {
                latestTime = jsonFileInfo.lastModified();
                latestFolderInfo = folderInfo;
            }
        }
    }

    if (latestFolderInfo.isDir()) {
        return latestFolderInfo.absoluteFilePath(); // 返回类似 ".../data/18" 的路径
    }

    return "";
}

QString SessionStorage::findUserAvatarPath(const QString& userId) const
{
    if (userId.isEmpty()) {
        return "";
    }

    // 1. 构建用户目录路径
    QString userDirPath = m_storagePath + "/" + userId;
    QDir userDir(userDirPath);

    if (!userDir.exists()) {
        return "";
    }

    // 2. 定义支持的头像格式
    QStringList nameFilters;
    nameFilters << "avatar.png" << "avatar.jpg" << "avatar.jpeg" << "avatar.bmp";

    // 3. 查找文件
    QFileInfoList fileList = userDir.entryInfoList(nameFilters, QDir::Files);

    if (!fileList.isEmpty()) {
        // 返回找到的第一个头像文件的绝对路径
        return fileList.first().absoluteFilePath();
    }

    // 没找到则返回空
    return "";
}
