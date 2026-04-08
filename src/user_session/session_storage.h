#ifndef SESSION_STORAGE_H
#define SESSION_STORAGE_H

#include "user_data.h"
#include <QObject>
#include <QJsonObject>

class SessionStorage : public QObject
{
    Q_OBJECT

public:
    explicit SessionStorage(QObject* parent = nullptr);

    // 配置
    void setStoragePath(const QString& path);
    //void setFileName(const QString& name);
     void setUserId(const QString& userId);
    QString storagePath() const;
     QString userId() const;
    //QString fileName() const;

    // 会话操作
    // bool saveSession(const UserData& userData);
    // UserData loadSession();
    // bool clearSession();
    // bool hasStoredSession() const;

    // 用户数据
    //bool saveUserData(const QString& userId, const QJsonObject& data);
     QJsonObject loadUserData() const;
    bool deleteUserData();

    // 保存完整的用户数据
    bool saveUserData(const QJsonObject& userData);

    //bool hasStoredUserData() const;

    QString findLastModifiedUserFile() const;
    bool hasAnyStoredUserData() const; // 检测是否存在任何用户数据

signals:
    // void sessionSaved(const QString& userId);
    // void sessionLoaded(const UserData& userData);
    // void sessionCleared();
    void userDataSaved();
    void userDataLoaded();
    void userDataCleared();
    void error(const QString& message);

private:
    QString getSessionFilePath() const;
    QString getUserDataFilePath() const;
    bool ensureDirectoryExists(const QString& path) const;
    bool writeJsonFile(const QString& filePath, const QJsonObject& json) const;
    QJsonObject readJsonFile(const QString& filePath) const;


    QString m_storagePath;
    QString m_fileName = "session.json";
    QString m_userId;
};

#endif // SESSION_STORAGE_H
