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
    // 断开所有连接
    if (m_messageDispatcher) {
        disconnect(m_messageDispatcher, nullptr, this, nullptr);
    }

    // 清理数据
    m_currentUser = UserData();
    m_userData = QJsonObject();

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
        disconnect(m_messageDispatcher, &MessageDispatcher::unregisterResponseReceived,  // 新增
                   this, &UserSession::processUnregisterResponse);
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
        connect(m_messageDispatcher, &MessageDispatcher::unregisterResponseReceived,  // 新增
                this, &UserSession::processUnregisterResponse);
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
        qDebug() << "UserSession::logout()：用户未登录，无需登出";
        return;
    }

    // 记录登出用户
    UserData oldUser = m_currentUser;

    // 通过MessageDispatcher发送登出请求 - 先发送请求
    if (m_messageDispatcher) {
        qDebug() << "通过MessageDispatcher发送登出请求:" << oldUser.username();
        emit logoutRequest();
    } else {
        qWarning() << "MessageDispatcher 未设置, 跳过服务器登出";

        // 如果没有MessageDispatcher，直接本地登出
        // 保存用户数据
        if (m_currentUser.isValid()) {
            m_storage->saveUserData(m_currentUser.userId(), m_userData);
        }

        // 清理会话
        m_storage->clearSession();

        // 重置当前用户
        m_currentUser = UserData();
        m_userData = QJsonObject();

        emit logoutSuccess();
        emit userChanged(m_currentUser);

        qDebug() << "用户登出:" << oldUser.username();
        return;
    }

    // 注意：这里不清空用户信息，等到服务器响应后再清空
    // 这样MessageDispatcher可以正确获取用户信息来发送登出请求
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
        // 登出成功 - 现在可以安全地清空本地用户信息了
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

        // 保存用户数据
        if (m_currentUser.isValid()) {
            m_storage->saveUserData(m_currentUser.userId(), m_userData);
        }

        m_storage->clearSession();
        m_currentUser = UserData();
        m_userData = QJsonObject();

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
bool UserSession::autoLoginFromSavedSession()
{
    if (!m_initialized) {
        qWarning() << "UserSession 未初始化，无法自动登录";
        return false;
    }

    if (isLoggedIn()) {
        qDebug() << "用户已登录，无需自动登录";
        return true;
    }

    // 尝试从保存的会话中加载用户
    if (m_storage->hasStoredSession()) {
        UserData savedUser = m_storage->loadSession();
        if (savedUser.isValid() && savedUser.isLoggedIn()) {
            // 加载用户数据
            QJsonObject userData = m_storage->loadUserData(savedUser.userId());

            // 设置当前用户
            m_currentUser = savedUser;
            m_currentUser.updateLastActiveTime();
            m_userData = userData;

            // 发出登录成功的信号
            emit loginSuccess(m_currentUser);
            emit userChanged(m_currentUser);

            qDebug() << "自动登录成功: 用户" << savedUser.username()
                     << "ID:" << savedUser.userId();
            return true;
        }
    }

    qDebug() << "没有有效的保存会话，自动登录失败";
    return false;
}

// 为了兼容，保留原有的 tryAutoLogin 方法
bool UserSession::tryAutoLogin()
{
    return autoLoginFromSavedSession();
}

void UserSession::unregisterUser(const QString& username, const QString& password)
{
    if (!m_initialized) {
        emit unregisterFailed("UserSession 未初始化");
        return;
    }

    if (username.isEmpty() || password.isEmpty()) {
        emit unregisterFailed("用户名或密码不能为空");
        return;
    }

    if (!m_messageDispatcher) {
        emit unregisterFailed("系统错误: 消息分发器未设置");
        qWarning() << "MessageDispatcher 未设置, 无法发送注销请求";
        return;
    }

    qDebug() << "通过MessageDispatcher发送注销请求:" << username;
    emit unregisterRequest(username, password);
}

void UserSession::processUnregisterResponse(const QString& responseData)
{
    qDebug() << "处理注销响应:" << responseData;

    if (responseData.startsWith("SUCCESS:")) {
        QStringList parts = responseData.mid(8).split(":");
        if (parts.size() >= 2) {
            QString username = parts[0];
            int userId = parts[1].toInt();

            // 注销成功后，清理当前用户信息
            if (m_currentUser.isValid() && m_currentUser.isLoggedIn()) {
                //m_storage->saveUserData(m_currentUser.userId(), m_userData);
                m_storage->deleteUserData(m_currentUser.userId());
            }

            m_storage->clearSession();
            m_currentUser = UserData();
            m_userData = QJsonObject();

            emit unregisterSuccess(username, userId);
            emit userChanged(m_currentUser);

            qDebug() << "用户注销成功:" << username << "用户ID:" << userId;
        } else {
            emit unregisterFailed("注销响应格式错误");
        }
    } else if (responseData.startsWith("ERROR:")) {
        QString errorMsg = responseData.mid(6);
        emit unregisterFailed(errorMsg);
    } else {
        emit unregisterFailed("注销失败: 未知响应格式");
    }
}
