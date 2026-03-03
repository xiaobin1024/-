#include "user_data.h"
#include <QJsonDocument>
#include <QDebug>

UserData::UserData()
    : m_role(UserRole::Guest)
    , m_loggedIn(false)
{
}

UserData::UserData(const QString& userId, const QString& username, UserRole role)
    : m_userId(userId)
    , m_username(username)
    , m_role(role)
    , m_loggedIn(false)
{
}

QString UserData::userId() const
{
    return m_userId;
}

QString UserData::username() const
{
    return m_username;
}

UserRole UserData::role() const
{
    return m_role;
}

bool UserData::isLoggedIn() const
{
    return m_loggedIn;
}

bool UserData::isPremium() const
{
    return m_role == UserRole::PremiumUser;
}

QDateTime UserData::loginTime() const
{
    return m_loginTime;
}

QDateTime UserData::lastActiveTime() const
{
    return m_lastActiveTime;
}

void UserData::setUserId(const QString& id)
{
    m_userId = id;
}

void UserData::setUsername(const QString& name)
{
    m_username = name;
}

void UserData::setRole(UserRole role)
{
    m_role = role;
}

void UserData::setLoggedIn(bool loggedIn)
{
    m_loggedIn = loggedIn;
    if (loggedIn && !m_loginTime.isValid()) {
        m_loginTime = QDateTime::currentDateTime();
    }
    updateLastActiveTime();
}

void UserData::setLoginTime(const QDateTime& time)
{
    m_loginTime = time;
}

void UserData::setLastActiveTime(const QDateTime& time)
{
    m_lastActiveTime = time;
}

void UserData::updateLastActiveTime()
{
    m_lastActiveTime = QDateTime::currentDateTime();
}

QJsonObject UserData::toJson() const
{
    QJsonObject json;
    json["userId"] = m_userId;
    json["username"] = m_username;
    json["role"] = static_cast<int>(m_role);
    json["loggedIn"] = m_loggedIn;

    if (m_loginTime.isValid()) {
        json["loginTime"] = m_loginTime.toString(Qt::ISODate);
    }

    if (m_lastActiveTime.isValid()) {
        json["lastActiveTime"] = m_lastActiveTime.toString(Qt::ISODate);
    }

    return json;
}

UserData UserData::fromJson(const QJsonObject& json)
{
    UserData userData;

    if (json.contains("userId")) {
        userData.m_userId = json["userId"].toString();
    }

    if (json.contains("username")) {
        userData.m_username = json["username"].toString();
    }

    if (json.contains("role")) {
        userData.m_role = static_cast<UserRole>(json["role"].toInt());
    }

    if (json.contains("loggedIn")) {
        userData.m_loggedIn = json["loggedIn"].toBool();
    }

    if (json.contains("loginTime")) {
        userData.m_loginTime = QDateTime::fromString(json["loginTime"].toString(), Qt::ISODate);
    }

    if (json.contains("lastActiveTime")) {
        userData.m_lastActiveTime = QDateTime::fromString(json["lastActiveTime"].toString(), Qt::ISODate);
    }

    return userData;
}

bool UserData::isValid() const
{
    return !m_userId.isEmpty() && !m_username.isEmpty();
}
