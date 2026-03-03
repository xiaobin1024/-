#include "user_session.h"
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include<QJsonDocument>
#include "message_dispatcher.h"

UserSession* UserSession::m_instance = nullptr;

UserSession::UserSession(QObject* parent)
    : QObject(parent)
    , m_storage(new SessionStorage(this))
    ,m_messageDispatcher(nullptr)
{
    qDebug() << "UserSession 创建";

    // 连接存储错误信号
    connect(m_storage, &SessionStorage::error, this, &UserSession::error);
}

UserSession::~UserSession()
{
    qDebug() << "UserSession 销毁";
}

void UserSession::setMessageDispatcher(MessageDispatcher* dispatcher)
{
    if (m_messageDispatcher == dispatcher) {
        return;
    }

    // 断开旧的连接
    if (m_messageDispatcher) {
        disconnect(m_messageDispatcher, &MessageDispatcher::loginResponseReceived,
                   this, &UserSession::processLoginResponse);
        disconnect(m_messageDispatcher, &MessageDispatcher::registerResponseReceived,
                   this, &UserSession::processRegisterResponse);
        disconnect(m_messageDispatcher, &MessageDispatcher::logoutResponseReceived,
                   this, &UserSession::processLogoutResponse);
        disconnect(m_messageDispatcher, &MessageDispatcher::errorOccurred,
                   this, &UserSession::error);
    }

    m_messageDispatcher = dispatcher;

    // 连接新的信号
    if (m_messageDispatcher) {
        connect(m_messageDispatcher, &MessageDispatcher::loginResponseReceived,
                this, &UserSession::processLoginResponse);
        connect(m_messageDispatcher, &MessageDispatcher::registerResponseReceived,
                this, &UserSession::processRegisterResponse);
        connect(m_messageDispatcher, &MessageDispatcher::logoutResponseReceived,
                this, &UserSession::processLogoutResponse);
        connect(m_messageDispatcher, &MessageDispatcher::errorOccurred,
                this, &UserSession::error);

        qDebug() << "MessageDispatcher 已设置到 UserSession";
    }
}

MessageDispatcher* UserSession::messageDispatcher() const
{
    return m_messageDispatcher;
}

UserSession* UserSession::instance()
{
    if (!m_instance) {
        m_instance = new UserSession();
    }
    return m_instance;
}

void UserSession::initialize()
{
    if (m_initialized) {
        qWarning() << "UserSession 已经初始化";
        return;
    }

    m_initialized = true;
    qDebug() << "UserSession 初始化完成";

    emit initialized();
}

bool UserSession::isInitialized() const
{
    return m_initialized;
}

UserData UserSession::currentUser() const
{
    return m_currentUser;
}

bool UserSession::isLoggedIn() const
{
    return m_currentUser.isLoggedIn() && m_currentUser.isValid();
}

bool UserSession::isGuest() const
{
    if (!m_currentUser.isValid()) {
        return true;
    }
    return m_currentUser.role() == UserRole::Guest;
}

bool UserSession::isFreeUser() const
{
    if (!m_currentUser.isValid()) {
        return false;
    }
    return m_currentUser.role() == UserRole::FreeUser;
}

bool UserSession::isPremiumUser() const
{
    if (!m_currentUser.isValid()) {
        return false;
    }
    return m_currentUser.role() == UserRole::PremiumUser;
}

void UserSession::login(const QString& username, const QString& password, bool rememberMe)
{
    if (!m_initialized) {
        emit loginFailed("UserSession 未初始化");
        return;
    }

    if (username.isEmpty() || password.isEmpty()) {
        emit loginFailed("用户名或密码不能为空");
        return;
    }
    // 生产环境：通过MessageDispatcher发送登录请求
    if (!m_messageDispatcher) {
        emit loginFailed("系统错误: 消息分发器未设置");
        qWarning() << "MessageDispatcher 未设置, 无法发送登录请求";
        return;
    }

    qDebug() << "通过MessageDispatcher发送登录请求:" << username;
    emit loginRequest(username, password);
}

void UserSession::logout()
{
    if (!isLoggedIn()) {
        return;
    }

    // 保存用户数据
    if (m_currentUser.isValid()) {
        m_storage->saveUserData(m_currentUser.userId(), m_userData);
    }

    // 清理会话
    m_storage->clearSession();

    // 记录登出用户
    UserData oldUser = m_currentUser;

    // 重置当前用户
    m_currentUser = UserData();
    m_userData = QJsonObject();

    // 通过MessageDispatcher发送登出请求
    if (m_messageDispatcher) {
        qDebug() << "通过MessageDispatcher发送登出请求:" << oldUser.username();
        emit logoutRequest();
    } else {
        qWarning() << "MessageDispatcher 未设置, 跳过服务器登出";
    }

    emit logoutSuccess();
    emit userChanged(m_currentUser);

    qDebug() << "用户登出:" << oldUser.username();
}

void UserSession::registerUser(const QString& username, const QString& password)
{
    if (!m_initialized) {
        emit registerFailed("UserSession 未初始化");
        return;
    }

    if (username.isEmpty() || password.isEmpty()) {
        emit registerFailed("用户名或密码不能为空");
        return;
    }

    if (username.length() < 3 || username.length() > 20) {
        emit registerFailed("用户名长度必须在3-20个字符之间");
        return;
    }

    if (password.length() < 6) {
        emit registerFailed("密码长度至少6个字符");
        return;
    }

    qDebug() << "通过MessageDispatcher发送注册请求:" << username;
    emit registerRequest(username, password);
}

void UserSession::processLoginResponse(const QString& responseData)
{
    if (!m_initialized) {
        emit loginFailed("UserSession 未初始化");
        return;
    }

    qDebug() << "处理登录响应:" << responseData;

    // 解析响应数据
    if (responseData.startsWith("SUCCESS:")) {
        QStringList parts = responseData.mid(8).split(":");
        if (parts.size() >= 3) {
            QString username = parts[0];
            QString userId = parts[1];
            int roleInt = parts[2].toInt();
            UserRole role = static_cast<UserRole>(roleInt);

            // 创建用户数据
            UserData userData(userId, username, role);
            userData.setLoggedIn(true);
            userData.updateLastActiveTime();

            m_currentUser = userData;

            // 修复1: 加载现有数据或初始化新数据
            m_userData = m_storage->loadUserData(userId);
            if (m_userData.isEmpty()) {
                qDebug() << "初始化新用户数据对象";
                m_userData["userId"] = userId;
                m_userData["username"] = username;
                m_userData["role"] = roleInt;
                // 保存基础数据
                m_storage->saveUserData(userId, m_userData);
            }

            // 修复2: 确保基础数据存在
            if (!m_userData.contains("userId")) {
                m_userData["userId"] = userId;
            }
            if (!m_userData.contains("username")) {
                m_userData["username"] = username;
            }
            if (!m_userData.contains("role")) {
                m_userData["role"] = roleInt;
            }


            // 保存会话
            bool rememberMe = (parts.size() > 3) ? (parts[3] == "1") : true;
            if (rememberMe) {
                m_storage->saveSession(m_currentUser);
            }

            emit loginSuccess(m_currentUser);
            emit userChanged(m_currentUser);

            qDebug() << "登录成功:" << username << "用户ID:" << userId;
            return;
        }
    } else if (responseData.startsWith("ERROR:")) {
        QString errorMsg = responseData.mid(6);
        emit loginFailed(errorMsg);
        return;
    }

    // 默认错误
    emit loginFailed("登录失败: 未知响应格式");
}

void UserSession::processLogoutResponse(const QString& responseData)
{
    qDebug() << "处理登出响应:" << responseData;

    if (responseData.startsWith("SUCCESS:")) {
        // 登出成功
        if (m_currentUser.isValid() && m_currentUser.isLoggedIn()) {
            m_storage->saveUserData(m_currentUser.userId(), m_userData);
        }

        m_storage->clearSession();

        UserData oldUser = m_currentUser;
        m_currentUser = UserData();
        m_userData = QJsonObject();

        emit logoutSuccess();
        emit userChanged(m_currentUser);

        qDebug() << "服务器登出成功:" << oldUser.username();
    } else {
        // 即使服务器登出失败，也要执行本地登出
        UserData oldUser = m_currentUser;
        m_currentUser = UserData();
        m_userData = QJsonObject();
        m_storage->clearSession();

        emit logoutSuccess();
        emit userChanged(m_currentUser);

        qWarning() << "服务器登出失败，执行本地登出:" << oldUser.username()
                   << "错误:" << responseData;
    }
}

void UserSession::processRegisterResponse(const QString& responseData)
{
    qDebug() << "处理注册响应:" << responseData;

    if (responseData.startsWith("SUCCESS:")) {
        // 注册成功，自动登录
        QStringList parts = responseData.mid(8).split(":");
        if (parts.size() >= 2) {
            QString username = parts[0];
            QString password = parts[1];
            qDebug() << "解析出的用户名:" << username;
            qDebug() << "解析出的密码:" << password;

            qDebug() << "发出 registerSuccess 信号...";
            emit registerSuccess(username);

            // 尝试自动登录
            emit loginRequest(username, password);
            qDebug() << "尝试自动登录，发出 loginRequest 信号...";
        } else {
            emit registerFailed("注册响应格式错误");
        }
    } else if (responseData.startsWith("ERROR:")) {
        QString errorMsg = responseData.mid(6);
        emit registerFailed(errorMsg);
    } else {
        emit registerFailed("注册失败: 未知响应格式");
    }
}

bool UserSession::tryAutoLogin()
{
    if (!m_initialized) {
        qWarning() << "尝试自动登录失败: UserSession 未初始化";
        return false;
    }

    if (m_storage->hasStoredSession()) {
        UserData user = m_storage->loadSession();
        if (user.isValid() && user.isLoggedIn()) {
            m_currentUser = user;
            m_currentUser.updateLastActiveTime();
            m_userData = m_storage->loadUserData(user.userId());

            emit loginSuccess(m_currentUser);
            emit userChanged(m_currentUser);

            qDebug() << "自动登录成功:" << user.username();
            return true;
        }
    }

    return false;
}

bool UserSession::canSaveWords() const
{
    return isLoggedIn();
}

bool UserSession::canCreateNotes() const
{
    return isLoggedIn();
}

bool UserSession::canExportData() const
{
    return isPremiumUser();
}

void UserSession::updateCurrentUser(const UserData& user)
{
    UserData oldUser = m_currentUser;
    m_currentUser = user;

    if (m_currentUser.isLoggedIn()) {
        m_currentUser.updateLastActiveTime();

        // 如果用户ID变化，加载对应的用户数据
        if (oldUser.userId() != user.userId()) {
            m_userData = m_storage->loadUserData(user.userId());
        }

        // 保存会话
        m_storage->saveSession(m_currentUser);
    }

    emit userChanged(m_currentUser);

    qDebug() << "用户数据已更新:" << user.username();
}

void UserSession::saveUserData(const QString& key, const QVariant& value)
{
    if (!isLoggedIn()) {
        qWarning() << "无法保存用户数据: 用户未登录";
        return;
    }

    m_userData[key] = QJsonValue::fromVariant(value);

    // 自动保存到文件
    m_storage->saveUserData(m_currentUser.userId(), m_userData);

    qDebug() << "用户数据已保存:" << key << "=" << value;
}

QVariant UserSession::getUserData(const QString& key, const QVariant& defaultValue) const
{
    if (!isLoggedIn()) {
        qWarning() << "无法获取用户数据: 用户未登录";
        return defaultValue;
    }

    if (!m_userData.contains(key)) {
        return defaultValue;
    }

    QJsonValue value = m_userData[key];
    if (value.isUndefined() || value.isNull()) {
        return defaultValue;
    }

    return value.toVariant();
}

QString UserSession::serializeUserData(const UserData& userData)
{
    QJsonObject json = userData.toJson();
    QJsonDocument doc(json);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

UserData UserSession::deserializeUserData(const QString& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return UserData();
    }

    return UserData::fromJson(doc.object());
}

void UserSession::reset()
{
    // 保存当前用户数据
    if (m_currentUser.isValid() && m_currentUser.isLoggedIn()) {
        if (!m_userData.isEmpty() && m_userData.contains("userId")) {
            qDebug() << "重置前保存用户数据";
            m_storage->saveUserData(m_currentUser.userId(), m_userData);
        }
    }

    // 清除所有数据
    m_currentUser = UserData();
    m_userData = QJsonObject();
    m_storage->clearSession();

    m_initialized = false;

    qDebug() << "UserSession 已重置";
}

bool UserSession::localAuthenticate(const QString& username, const QString& password, UserData& userData)
{
    // 开发环境下的简单本地认证
    static QMap<QString, QPair<QString, UserRole>> userDatabase = {
        {"admin", {"admin123", UserRole::PremiumUser}},
        {"user1", {"password1", UserRole::FreeUser}},
        {"user2", {"password2", UserRole::FreeUser}},
        {"test", {"test", UserRole::PremiumUser}}
    };

    if (!userDatabase.contains(username)) {
        qDebug() << "本地认证失败: 用户不存在" << username;
        return false;
    }

    auto& userInfo = userDatabase[username];
    if (userInfo.first != password) {
        qDebug() << "本地认证失败: 密码错误" << username;
        return false;
    }

    // 认证成功，创建用户数据
    QString userId = QString("%1_%2").arg(username).arg(QDateTime::currentMSecsSinceEpoch());
    userData = UserData(userId, username, userInfo.second);
    userData.setLoggedIn(true);
    userData.updateLastActiveTime();

    qDebug() << "本地认证成功:" << username << "用户类型:" << static_cast<int>(userInfo.second);
    return true;
}
