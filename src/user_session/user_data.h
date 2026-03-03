#ifndef USER_DATA_H
#define USER_DATA_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

// 用户角色
enum class UserRole {
    Guest,      // 游客
    FreeUser,   // 免费用户
    PremiumUser // 付费用户
};

class UserData
{
public:
    UserData();
    explicit UserData(const QString& userId, const QString& username, UserRole role = UserRole::Guest);

    // 基本属性
    QString userId() const;
    QString username() const;
    UserRole role() const;
    bool isLoggedIn() const;
    bool isPremium() const;

    // 时间信息
    QDateTime loginTime() const;
    QDateTime lastActiveTime() const;

    // 设置方法
    void setUserId(const QString& id);
    void setUsername(const QString& name);
    void setRole(UserRole role);
    void setLoggedIn(bool loggedIn);
    void setLoginTime(const QDateTime& time);
    void setLastActiveTime(const QDateTime& time);

    // 更新活动时间
    void updateLastActiveTime();

    // JSON 序列化/反序列化
    QJsonObject toJson() const;
    static UserData fromJson(const QJsonObject& json);

    // 验证
    bool isValid() const;

private:
    QString m_userId;
    QString m_username;
    UserRole m_role = UserRole::Guest;
    QDateTime m_loginTime;
    QDateTime m_lastActiveTime;
    bool m_loggedIn = false;
};

#endif // USER_DATA_H
