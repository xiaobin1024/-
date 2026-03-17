#include "system_sidebar.h"
#include "theme_manager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include<QMessageBox>
#include<QInputDialog>


SystemSidebar::SystemSidebar(QWidget* parent)
    : BaseWidget(parent)
    , m_widthAnimation(new QPropertyAnimation(this, "currentWidth"))
    , m_opacityAnimation(new QPropertyAnimation(this, "windowOpacity"))
    , m_messageDispatcher(nullptr)
    , m_userSession(nullptr)
{
    // 设置对象名，方便样式定制
    setObjectName("SystemSidebar");

    // 设置初始尺寸
    setFixedWidth(COLLAPSED_WIDTH);  // 初始为收起宽度

    // 设置初始收起状态
    m_expanded = false;
    m_currentWidth = COLLAPSED_WIDTH;

    initialize();


    // 初始化动画
    setupAnimations();

    qDebug() << "SystemSidebar 初始化完成，初始状态：收起";
}

void SystemSidebar::setUserSession(UserSession* session)
{
    if (m_userSession == session) {
        return;
    }

    // 断开旧连接
    if (m_userSession) {
        disconnect(m_userSession, &UserSession::userChanged,
                   this, &SystemSidebar::onUserLoggedIn);
        disconnect(m_userSession, &UserSession::logoutSuccess,
                   this, &SystemSidebar::onUserLoggedOut);
        disconnect(m_userSession, &UserSession::loginFailed,
                   this, &SystemSidebar::onUserLoginFailed);
        disconnect(m_userSession, &UserSession::logoutSuccess,
                   this, &SystemSidebar::onUserLogoutSuccess);
        disconnect(m_userSession, &UserSession::unregisterSuccess,  // 新增
                   this, &SystemSidebar::onUserUnregisterSuccess);
        disconnect(m_userSession, &UserSession::unregisterFailed,  // 新增
                   this, &SystemSidebar::onUserUnregisterFailed);
        disconnect(m_userSession, &UserSession::userChanged, this, &SystemSidebar::onUserChanged);
    }

    m_userSession = session;

    // 连接新连接
    if (m_userSession&& m_userSession != UserSession::instance()) {
        connect(m_userSession, &UserSession::userChanged,
                this, &SystemSidebar::onUserLoggedIn);
        connect(m_userSession, &UserSession::logoutSuccess,
                this, &SystemSidebar::onUserLoggedOut);
        connect(m_userSession, &UserSession::loginFailed,
                this, &SystemSidebar::onUserLoginFailed);
        connect(m_userSession, &UserSession::logoutSuccess,
                this, &SystemSidebar::onUserLogoutSuccess);
        connect(m_userSession, &UserSession::unregisterSuccess,  // 新增
                this, &SystemSidebar::onUserUnregisterSuccess);
        connect(m_userSession, &UserSession::unregisterFailed,  // 新增
                this, &SystemSidebar::onUserUnregisterFailed);
        connect(m_userSession, &UserSession::userChanged, this, &SystemSidebar::onUserChanged);

        // 立即更新当前用户状态
        UserData currentUser = m_userSession->currentUser();
        onUserLoggedIn(currentUser);
    }
}
void SystemSidebar::onUserChanged(const UserData& user)
{
    if (!m_logoutButton || !m_deleteButton) {
        return;
    }

    if (user.isLoggedIn() && !user.username().isEmpty()) {
        m_logoutButton->setText(QString("🚪 退出").arg(user.username()));
        m_logoutButton->setEnabled(true);
        m_deleteButton->setEnabled(true);
    } else {
        m_logoutButton->setText("🚪 未登录");
        m_logoutButton->setEnabled(false);
        m_deleteButton->setEnabled(false);
    }

    updateUserButtonsState();
}
void SystemSidebar::setMessageDispatcher(MessageDispatcher* dispatcher)
{
    m_messageDispatcher = dispatcher;
    if (m_userSession && m_messageDispatcher) {
        m_messageDispatcher->setUserSession(m_userSession);
        m_userSession->setMessageDispatcher(m_messageDispatcher);
    }
}

void SystemSidebar::setupLayout()
{
    qDebug()<<"SystemSidebar::setupLayout()";
    // 1. 确保主布局已存在（由 BaseWidget 创建）
    if (!m_mainLayout) {
        qWarning() << "主布局未创建";
        return;
    }
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 2. 创建主按钮容器
    m_buttonContainer = new QWidget(this);
    m_buttonContainer->setObjectName("buttonContainer");
    m_buttonContainer->setFixedWidth(EXPANDED_WIDTH);

    // 3. 创建主按钮布局（垂直布局）
    m_buttonLayout = new QVBoxLayout(m_buttonContainer);
    m_buttonLayout->setContentsMargins(8, 12, 8, 12);
    m_buttonLayout->setSpacing(10);  // 增加按钮间距

    // 4. 创建顶部功能按钮容器 - 修复：作为成员变量或确保生命周期
    m_topButtons = new QWidget(m_buttonContainer);  // 作为成员变量
    m_topLayout = new QVBoxLayout(m_topButtons);
    m_topLayout->setContentsMargins(0, 0, 0, 0);
    m_topLayout->setSpacing(10);
    m_topLayout->setAlignment(Qt::AlignTop);

    // 5. 创建底部收起按钮容器 - 修复：作为成员变量
    m_bottomContainer = new QWidget(m_buttonContainer);  // 作为成员变量
    m_bottomLayout = new QVBoxLayout(m_bottomContainer);
    m_bottomLayout->setContentsMargins(0, 0, 0, 0);
    m_bottomLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    // 6. 创建中间弹性空间
    m_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    // 7. 创建所有按钮
    createButtons();

    // 8. 将按钮添加到相应容器
    m_topLayout->addWidget(m_themeButton);
    m_topLayout->addWidget(m_logoutButton);
    m_topLayout->addWidget(m_deleteButton);

    m_bottomLayout->addWidget(m_toggleButton);

    // 9. 将容器添加到主布局
    m_buttonLayout->addWidget(m_topButtons);
    m_buttonLayout->addSpacerItem(m_spacer);
    m_buttonLayout->addWidget(m_bottomContainer);

    // 10. 将按钮容器添加到主布局
    m_mainLayout->addWidget(m_buttonContainer);

    // 11. 应用初始样式
    updateSidebarStyle();

    qDebug() << "SystemSidebar 布局设置完成";
}

void SystemSidebar::createButtons()
{
    // 安全检查：如果按钮已存在，先清理
    if (m_themeButton) m_themeButton->deleteLater();
    if (m_logoutButton) m_logoutButton->deleteLater();
    if (m_deleteButton) m_deleteButton->deleteLater();
    if (m_toggleButton) m_toggleButton->deleteLater();

    // 使用 BaseWidget 提供的标准按钮创建方法
    m_themeButton = createSecondaryButton("", "themeButton");
    m_logoutButton = createSecondaryButton("", "logoutButton");
    m_deleteButton = createSecondaryButton("", "deleteButton");
    m_toggleButton = createSecondaryButton("", "toggleButton");


    // 设置按钮属性
    m_themeButton->setProperty("sidebarButton", true);
    m_logoutButton->setProperty("sidebarButton", true);
    m_deleteButton->setProperty("sidebarButton", true);
    m_toggleButton->setProperty("sidebarButton", true);

    // 设置按钮光标
    m_themeButton->setCursor(Qt::PointingHandCursor);
    m_logoutButton->setCursor(Qt::PointingHandCursor);
    m_deleteButton->setCursor(Qt::PointingHandCursor);
    m_toggleButton->setCursor(Qt::PointingHandCursor);

    // 连接信号
    connect(m_themeButton, &QPushButton::clicked,
            this, &SystemSidebar::handleThemeButtonClicked);
    connect(m_logoutButton, &QPushButton::clicked,
            this, &SystemSidebar::handleLogoutButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked,
            this, &SystemSidebar::handleDeleteAccountButtonClicked);
    connect(m_toggleButton, &QPushButton::clicked,
            this, [this]() { toggle(); });

    qDebug() << "SystemSidebar 按钮创建完成";
}

void SystemSidebar::setupAnimations()
{
    // 设置宽度动画
    m_widthAnimation->setDuration(300); // 300ms动画
    m_widthAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_widthAnimation, &QPropertyAnimation::finished,
            this, &SystemSidebar::onAnimationFinished);

    // 设置透明度动画
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    qDebug() << "SystemSidebar 动画设置完成";
}

void SystemSidebar::setExpanded(bool expanded)
{
    if (m_expanded == expanded) {
        return;
    }

    m_expanded = expanded;
    qDebug() << "SystemSidebar 设置展开状态：" << (expanded ? "展开" : "收起");

    // 启动动画
    startExpandAnimation(expanded);

    // 发射状态变化信号
    emit expandedChanged(expanded);
}

void SystemSidebar::toggle()
{
    setExpanded(!m_expanded);
}

void SystemSidebar::setCurrentWidth(int width)
{
    if (m_currentWidth != width) {
        m_currentWidth = width;
        setFixedWidth(width);

        // 宽度变化时更新按钮文本可见性
        updateSidebarStyle();
    }
}

void SystemSidebar::handleThemeButtonClicked()
{
    qDebug() << "主题切换按钮被点击";

    // 通过全局ThemeManager切换主题
    ThemeManager::instance()->toggleTheme();

    // 发射主题切换信号
    emit themeToggleRequested();

    // 显示操作反馈
    showMessage("主题切换中...", false, 800);
}

void SystemSidebar::handleLogoutButtonClicked()
{
    qDebug() << "退出登录按钮被点击";

    // 确认对话框（可选）
    if (m_userSession && m_userSession->isLoggedIn()) {
        // 通过 UserSession 发起登出
        qDebug() << "用户已登录，发起登出请求";
        m_userSession->logout();
    } else {
        qDebug() << "用户未登录，当前登录状态:" << (m_userSession ? m_userSession->isLoggedIn() : false);
        showMessage("当前无用户登录", false, 1500);
    }
}

void SystemSidebar::handleDeleteAccountButtonClicked()
{
    qDebug() << "注销账号按钮被点击";

    if (m_userSession && m_userSession->isLoggedIn()) {
        // 显示确认对话框
        UserData currentUser = m_userSession->currentUser();
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "确认注销",
            QString("确定要注销账户 '%1' 吗？此操作不可撤销！").arg(currentUser.username()),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            // 执行注销操作 - 这里需要提供正确的密码
            // 由于我们没有直接访问密码的机制，这里需要修改设计
            // 一种方案是让用户再次输入密码
            bool ok;
            QString password = QInputDialog::getText(
                this,
                "确认密码",
                "请输入密码以确认注销:",
                QLineEdit::Password,
                "",
                &ok
                );

            if (ok && !password.isEmpty()) {
                m_userSession->unregisterUser(currentUser.username(), password);
                showMessage("正在注销账户...", false, 2000);
            } else {
                showMessage("取消注销操作", false, 1500);
            }
        }
    } else {
        showMessage("请先登录后再操作", false, 1500);
    }
}

// 添加注销成功处理槽函数
void SystemSidebar::onUserUnregisterSuccess(const QString& username, int userId)
{
    showMessage(QString("账户 '%1' 注销成功！").arg(username), false, 2000);
    qDebug() << "账户注销成功:" << username << "用户ID:" << userId;

    // 更新按钮状态
    updateUserButtonsState();
}

// 添加注销失败处理槽函数
void SystemSidebar::onUserUnregisterFailed(const QString& error)
{
    showMessage("注销失败：" + error, true, 2000);
    qDebug() << "账户注销失败:" << error;

    // 保持当前状态不变
    updateUserButtonsState();
}

void SystemSidebar::startExpandAnimation(bool expand)
{
    // 修复3: 检查动画是否有效
    if (!m_widthAnimation) {
        qWarning() << "宽度动画对象无效";
        return;
    }

    if (m_widthAnimation->state() == QPropertyAnimation::Running) {
        m_widthAnimation->stop();
        // 不要立即删除，等待当前事件处理完成
    }

    int startWidth = m_currentWidth;
    int endWidth = expand ? EXPANDED_WIDTH : COLLAPSED_WIDTH;

    m_widthAnimation->setStartValue(startWidth);
    m_widthAnimation->setEndValue(endWidth);
    m_widthAnimation->start();

    qDebug() << "启动侧边栏动画：" << startWidth << " -> " << endWidth;
}

void SystemSidebar::onAnimationFinished()
{
    qDebug() << "侧边栏动画完成，当前宽度：" << m_currentWidth;

    // 动画完成后确保按钮状态更新
    updateSidebarStyle();
}

void SystemSidebar::onThemeChanged()
{
    // 调用基类处理
    BaseWidget::onThemeChanged();

    // 更新侧边栏特有样式
    updateSidebarStyle();

    qDebug() << "SystemSidebar 主题更新完成";
}

void SystemSidebar::onPageShow()
{
    BaseWidget::onPageShow();
    qDebug() << "SystemSidebar 页面显示";
}

void SystemSidebar::onPageHide()
{
    BaseWidget::onPageHide();
    qDebug() << "SystemSidebar 页面隐藏";
}

void SystemSidebar::paintEvent(QPaintEvent* event)
{
    // 调用基类绘制
    BaseWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 使用m_expanded标志位来判断是否展开，而不是依赖当前宽度
    if (m_expanded && width() > 1) { // 只在展开状态下绘制分割线
        QPen pen;
        if (uiTheme() == UITheme::Light) {
            pen.setColor(QColor("#d0d0d0")); // 亮色主题的分割线
        } else {
            pen.setColor(QColor("#555555")); // 暗色主题的分割线
        }
        pen.setWidth(2);
        painter.setPen(pen);

        // 在侧边栏右边绘制分割线
        painter.drawLine(width()-2, 0, width()-2, height());
    }
}

void SystemSidebar::updateSidebarStyle()
{
    // 获取ThemeManager实例
    if (!ThemeManager::safeInstance()) {
        return; // ThemeManager 已销毁
    }

    // 检查按钮是否已初始化
    if (!m_themeButton || !m_logoutButton || !m_deleteButton || !m_toggleButton) {
        qWarning() << "按钮未初始化，跳过样式更新";
        return;
    }

    // 根据主题设置不同的背景色
    QString bgColor;
    if (uiTheme() == UITheme::Light) {
        // 亮色主题：侧边栏使用更明显的灰白色背景
        bgColor = "#f0f0f0";
    } else {
        // 暗色主题：侧边栏使用更明显的深灰色背景
        bgColor = "#3a3a3a";
    }

    // 根据展开状态决定按钮文本
    bool isExpandedState = m_expanded;

    // 更新按钮文本和图标
    if (isExpandedState) {
        // 展开状态：显示完整文本
        m_themeButton->setText(uiTheme() == UITheme::Light ? "🌙 暗色模式" : "☀️ 亮色模式");
        // 根据登录状态更新登出按钮文本
        if (m_userSession && m_userSession->isLoggedIn()) {
            m_logoutButton->setText("🚪 退出登录");
        } else {
            m_logoutButton->setText("🚪 未登录");
        }
        m_deleteButton->setText("🗑️ 注销账号");
        m_toggleButton->setText("⚙️ 收起");

        // 清除工具提示（文本已显示）
        m_themeButton->setToolTip("");
        m_logoutButton->setToolTip("");
        m_deleteButton->setToolTip("");
        m_toggleButton->setToolTip("收起侧边栏");

        // 显示所有按钮
        m_themeButton->setVisible(true);
        m_logoutButton->setVisible(true);
        m_deleteButton->setVisible(true);
        m_toggleButton->setVisible(true);
    } else {
        // 收起状态：只显示切换按钮
        m_themeButton->setText("");
        m_logoutButton->setText("");
        m_deleteButton->setText("");
        m_toggleButton->setText("☰"); // 使用菜单图标

        // 设置工具提示
        m_themeButton->setToolTip("切换主题");
        m_logoutButton->setToolTip("退出登录");
        m_deleteButton->setToolTip("注销账号");
        m_toggleButton->setToolTip("展开侧边栏");

        // 隐藏除切换按钮外的所有按钮
        m_themeButton->setVisible(false);
        m_logoutButton->setVisible(false);
        m_deleteButton->setVisible(false);
        m_toggleButton->setVisible(true); // 只显示切换按钮
    }

    // 构建侧边栏样式，不包含边框（边框在paintEvent中绘制）
    QString style = QString(
                        "SystemSidebar {"
                        "  background-color: %1;"
                        "  padding: 0;"
                        "  margin: 0;"
                        "}"
                        "#buttonContainer {"
                        "  background-color: transparent;"
                        "  border: none;"
                        "}"
                        ).arg(bgColor);

    // 应用样式表
    if (m_currentStyleSheet != style) {
        setStyleSheet(style);
        m_currentStyleSheet = style;
    }

    // 更新用户按钮状态
    updateUserButtonsState();

    // 强制重绘以确保分割线正确显示
    update();

    qDebug() << "SystemSidebar 样式更新完成，展开状态：" << isExpandedState;
}
void SystemSidebar::resizeEvent(QResizeEvent* event)
{
    BaseWidget::resizeEvent(event);

    // 调整按钮容器大小
    if (m_buttonContainer) {
        m_buttonContainer->setFixedWidth(width());
    }
}

SystemSidebar::~SystemSidebar()
{
    // 修复1: 确保动画完全停止
    if (m_widthAnimation) {
        m_widthAnimation->stop();
        m_widthAnimation->setDuration(0); // 立即跳到结束值
        m_widthAnimation->start();
        m_widthAnimation->deleteLater(); // 延迟删除
    }
    if (m_opacityAnimation) {
        m_opacityAnimation->stop();
        m_opacityAnimation->deleteLater();
    }

    // 断开与 UserSession 的连接
    if (m_userSession) {
        disconnect(m_userSession, &UserSession::userChanged,
                   this, &SystemSidebar::onUserLoggedIn);
        disconnect(m_userSession, &UserSession::logoutSuccess,
                   this, &SystemSidebar::onUserLoggedOut);
        disconnect(m_userSession, &UserSession::loginFailed,
                   this, &SystemSidebar::onUserLoginFailed);
        disconnect(m_userSession, &UserSession::logoutSuccess,
                   this, &SystemSidebar::onUserLogoutSuccess);
    }

    // MessageDispatcher 会被 parent-child 关系自动删除（因为设置了 this 作为 parent）
    // 所以不需要手动删除 m_messageDispatcher

    // 修复2: 确保在ThemeManager销毁前注销
    if (ThemeManager* tm = ThemeManager::safeInstance()) {
        tm->unregisterWidget(this);
    }

    qDebug() << "SystemSidebar 析构完成";
}

void SystemSidebar::onUserLoggedIn(const UserData& user)
{
    if (user.isLoggedIn()) {
        //showMessage(QString("欢迎回来，%1!").arg(user.username()), false, 1500);
        qDebug() << "用户登录：" << user.username();
    }

    updateUserButtonsState();
}
void SystemSidebar::onUserLoggedOut()
{
    showMessage("已退出登录", false, 1500);
    qDebug() << "用户已登出";

    updateUserButtonsState();
}
void SystemSidebar::onUserLoginFailed(const QString& error)
{
    showMessage("登录失败：" + error, true, 2000);
    qDebug() << "登录失败：" << error;
}

void SystemSidebar::onUserLogoutSuccess()
{
    showMessage("已成功退出登录", false, 1500);
    qDebug() << "登出成功";

    updateUserButtonsState();
}
void SystemSidebar::updateUserButtonsState()
{
    if (!m_logoutButton || !m_deleteButton) {
        return;
    }

    bool loggedIn = m_userSession && m_userSession->isLoggedIn();
    QString username = m_userSession ? m_userSession->currentUser().username() : "";

    // 根据登录状态启用/禁用按钮
    m_logoutButton->setEnabled(loggedIn);
    m_deleteButton->setEnabled(loggedIn);

    // 更新按钮文本（根据展开状态）
    bool isExpandedState = m_expanded && (m_currentWidth > COLLAPSED_WIDTH + 20);
    if (isExpandedState) {
        if (loggedIn && !username.isEmpty()) {
            m_logoutButton->setText(QString("🚪 退出").arg(username));
        } else {
            m_logoutButton->setText("🚪 未登录");
        }
        m_deleteButton->setText(loggedIn ? "🗑️ 注销账号" : "🗑️ 请登录");
    } else {
        // 收起状态只显示图标
        m_logoutButton->setText("");
        m_deleteButton->setText("");
    }

    // 更新按钮样式
    updateWidgetStyles();
}
