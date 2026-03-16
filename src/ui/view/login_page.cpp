#include "login_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QRegularExpressionValidator>
#include <QDebug>

LoginPage::LoginPage(QWidget* parent)
    : BaseWidget(parent)
    , m_userSession(UserSession::instance())
    , m_layoutSetupDone(false)
{
    qDebug() << "LoginPage 创建";
    initialize();
    // 初始化 UserSession
    if (!m_userSession->isInitialized()) {
        m_userSession->initialize();
    }

    connectSignals();
    //setupLayout();
}

LoginPage::~LoginPage()
{
    qDebug() << "LoginPage 销毁";
    if (m_userSession) {
        m_userSession->disconnect(this);
    }
}

void LoginPage::initUI()
{
    BaseWidget::initUI();
    setObjectName("LoginPage");

    // 设置页面标题
    setWindowTitle("登录 - 网络词典");
}

void LoginPage::setupLayout()
{
    if (m_layoutSetupDone) {
        return; // 防止重复初始化
    }
    qDebug()<<"LoginPage::setupLayout()";


    // 创建主布局容器
    auto* mainContainer = new QWidget(this);
    auto* mainContainerLayout = new QVBoxLayout(mainContainer);
    mainContainerLayout->setContentsMargins(0, 0, 0, 0);
    mainContainerLayout->setSpacing(20);

    // 标题
    m_titleLabel = createLabel("用户登录", "title", "titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString(
                                    "QLabel {"
                                    "  font-size: 24px;"
                                    "  font-weight: bold;"
                                    "  color: %1;"
                                    "  margin: 20px 0 30px 0;"
                                    "}"
                                    ).arg(getColor("text-primary")));

    // 登录表单组
    m_loginGroupBox = createGroupBox("登录信息", "loginGroupBox");
    m_formLayout = new QFormLayout(m_loginGroupBox);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_formLayout->setLabelAlignment(Qt::AlignRight);
    m_formLayout->setFormAlignment(Qt::AlignCenter);
    m_formLayout->setSpacing(15);
    m_formLayout->setContentsMargins(20, 20, 20, 20);

    setupLoginForm();
    setupButtons();
    setupFooter();

    // 添加到主容器
    mainContainerLayout->addWidget(m_titleLabel, 0, Qt::AlignTop);
    mainContainerLayout->addWidget(m_loginGroupBox, 0, Qt::AlignCenter);
    mainContainerLayout->addStretch();

    // 将主容器添加到基类布局
    m_mainLayout->addWidget(mainContainer, 0, Qt::AlignCenter);

    m_layoutSetupDone = true;
}

void LoginPage::setupLoginForm()
{
    // 用户名输入
    m_usernameEdit = createLineEdit("请输入用户名", "usernameEdit");
    m_usernameEdit->setObjectName("usernameEdit");
    m_usernameEdit->setMinimumWidth(300);
    m_usernameEdit->setMaximumWidth(400);

    // 设置用户名输入验证器
    QRegularExpression regex("^[a-zA-Z0-9_]{3,20}$");
    auto* validator = new QRegularExpressionValidator(regex, this);
    m_usernameEdit->setValidator(validator);

    // 密码输入
    m_passwordEdit = createLineEdit("请输入密码", "passwordEdit");
    m_passwordEdit->setObjectName("passwordEdit");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumWidth(300);
    m_passwordEdit->setMaximumWidth(400);

    // 记住我复选框
    m_rememberMeCheckBox = new QCheckBox("记住登录状态", this);
    m_rememberMeCheckBox->setObjectName("rememberMeCheckBox");
    m_rememberMeCheckBox->setChecked(true);
    m_rememberMeCheckBox->setStyleSheet(QString(
                                            "QCheckBox {"
                                            "  color: %1;"
                                            "  font-size: 14px;"
                                            "}"
                                            "QCheckBox::indicator {"
                                            "  width: 18px;"
                                            "  height: 18px;"
                                            "  border: 1px solid %2;"
                                            "  border-radius: 3px;"
                                            "  background-color: %3;"
                                            "}"
                                            "QCheckBox::indicator:checked {"
                                            "  background-color: %4;"
                                            "}"
                                            ).arg(getColor("text-primary"),
                                                 getColor("border"),
                                                 getColor("surface"),
                                                 getColor("primary")));

    // 添加到表单布局
    m_formLayout->addRow("用户名:", m_usernameEdit);
    m_formLayout->addRow("密码:", m_passwordEdit);
    m_formLayout->addRow(m_rememberMeCheckBox);

    // 连接输入验证
    connect(m_usernameEdit, &QLineEdit::textChanged, this, &LoginPage::validateInput);
    connect(m_passwordEdit, &QLineEdit::textChanged, this, &LoginPage::validateInput);
}

void LoginPage::setupButtons()
{
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->setAlignment(Qt::AlignCenter);

    // 登录按钮
    m_loginButton = createPrimaryButton("登录", "loginButton");
    m_loginButton->setMinimumWidth(120);
    m_loginButton->setMinimumHeight(40);
    m_loginButton->setEnabled(false); // 初始禁用，直到输入有效

    // 注册按钮
    m_registerButton = createSecondaryButton("注册账户", "registerButton");
    m_registerButton->setMinimumWidth(120);
    m_registerButton->setMinimumHeight(40);

    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_registerButton);

    m_formLayout->addRow(buttonLayout);
}

void LoginPage::setupFooter()
{
    auto* footerLayout = new QHBoxLayout();
    footerLayout->setAlignment(Qt::AlignCenter);

    // 忘记密码按钮
    m_forgotPasswordButton = createSecondaryButton("忘记密码?", "forgotPasswordButton");
    m_forgotPasswordButton->setFlat(true);
    m_forgotPasswordButton->setStyleSheet(QString(
                                              "QPushButton {"
                                              "  color: %1;"
                                              "  font-size: 12px;"
                                              "  text-decoration: underline;"
                                              "  border: none;"
                                              "  padding: 5px;"
                                              "}"
                                              "QPushButton:hover {"
                                              "  color: %2;"
                                              "}"
                                              ).arg(getColor("text-secondary"), getColor("primary")));

    footerLayout->addWidget(m_forgotPasswordButton);

    m_formLayout->addRow(footerLayout);

    // 连接按钮信号
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginButtonClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterButtonClicked);
    connect(m_forgotPasswordButton, &QPushButton::clicked, this, &LoginPage::onForgotPasswordClicked);
}

void LoginPage::connectSignals() {

    if (m_signalsConnected) {
        qDebug() << "LoginPage 信号已连接，跳过重复连接";
        return;
    }

    // UserSession 信号连接
    bool connected1 = connect(m_userSession, &UserSession::loginSuccess,
                              this, &LoginPage::onLoginSuccess);
    bool connected2 = connect(m_userSession, &UserSession::loginFailed,
                              this, &LoginPage::onLoginFailed);
    bool connected3 = connect(m_userSession, &UserSession::userChanged,
                              this, &LoginPage::onUserChanged);
     m_signalsConnected = true;

    qDebug() << "LoginPage 信号连接状态:"
             << "loginSuccess:" << connected1
             << "loginFailed:" << connected2
             << "userChanged:" << connected3;
}

void LoginPage::onLoginButtonClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    bool rememberMe = m_rememberMeCheckBox->isChecked();

    if (username.isEmpty() || password.isEmpty()) {
        showMessage("用户名和密码不能为空", true, 2000);
        return;
    }

    // 验证用户名格式
    QRegularExpression regex("^[a-zA-Z0-9_]{3,20}$");
    if (!regex.match(username).hasMatch()) {
        showMessage("用户名格式不正确（3-20个字符，字母数字下划线）", true, 2000);
        return;
    }

    // 禁用UI防止重复点击
    setUIState(false);
    showLoading("正在登录...");

    // 发起登录请求
    m_userSession->login(username, password, rememberMe);
}

void LoginPage::onRegisterButtonClicked()
{
    emit navigateToRegister();
}

void LoginPage::onForgotPasswordClicked()
{
    showMessage("忘记密码功能开发中...", false, 1500);
    // TODO: 实现忘记密码功能
}

void LoginPage::onLoginSuccess(const UserData& user) {

    qDebug() << "LoginPage::onLoginSuccess 被调用，用户名:" << user.username();

    hideLoading();
    setUIState(true);

    showMessage(QString("登录成功！欢迎回来，%1").arg(user.username()), false, 1500);

    emit loginSuccess(user);
    qDebug() << "LoginPage 发射 loginSuccess 信号";

    m_lastLoggedInUser = user;
    m_processingLogin = false;

    emit navigateToMain();
}
void LoginPage::onLoginFailed(const QString& error)
{
    hideLoading();
    setUIState(true);

    showMessage("登录失败：" + error, true, 3000);

    // 清空密码字段
    m_passwordEdit->clear();
    m_passwordEdit->setFocus();
}

void LoginPage::onUserChanged(const UserData& user)
{
    if (user.isLoggedIn()) {
        onLoginSuccess(user);
    } else if (!user.isValid()) {
        // 用户登出或注销
        // 可以在这里做一些清理工作
    }
}

void LoginPage::validateInput()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    bool valid = !username.isEmpty() && !password.isEmpty() &&
                 username.length() >= 3 && username.length() <= 20 &&
                 password.length() >= 6;

    m_loginButton->setEnabled(valid);
}

void LoginPage::setUIState(bool enabled)
{
    m_usernameEdit->setEnabled(enabled);
    m_passwordEdit->setEnabled(enabled);
    m_rememberMeCheckBox->setEnabled(enabled);
    m_loginButton->setEnabled(enabled && m_usernameEdit->text().trimmed().length() >= 3
                              && m_passwordEdit->text().length() >= 6);
    m_registerButton->setEnabled(enabled);
    m_forgotPasswordButton->setEnabled(enabled);
}

void LoginPage::setLoginData(const QString& username, bool rememberMe)
{
    //安全检查
    if (!m_usernameEdit || !m_rememberMeCheckBox) {
        qWarning() << "LoginPage UI 未初始化完成";
        return;
    }
    m_initializing = true;

    m_usernameEdit->setText(username);
    m_rememberMeCheckBox->setChecked(rememberMe);

    if (!username.isEmpty()) {
        m_passwordEdit->setFocus();
    } else {
        m_usernameEdit->setFocus();
    }

    validateInput();

    m_initializing = false;
}

void LoginPage::updateWidgetStyles()
{
    BaseWidget::updateWidgetStyles();

    // 更新特定控件样式
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString(
                                        "QLabel {"
                                        "  font-size: 24px;"
                                        "  font-weight: bold;"
                                        "  color: %1;"
                                        "  margin: 20px 0 30px 0;"
                                        "}"
                                        ).arg(getColor("text-primary")));
    }

    if (m_rememberMeCheckBox) {
        m_rememberMeCheckBox->setStyleSheet(QString(
                                                "QCheckBox {"
                                                "  color: %1;"
                                                "  font-size: 14px;"
                                                "}"
                                                "QCheckBox::indicator {"
                                                "  width: 18px;"
                                                "  height: 18px;"
                                                "  border: 1px solid %2;"
                                                "  border-radius: 3px;"
                                                "  background-color: %3;"
                                                "}"
                                                "QCheckBox::indicator:checked {"
                                                "  background-color: %4;"
                                                "}"
                                                ).arg(getColor("text-primary"),
                                                     getColor("border"),
                                                     getColor("surface"),
                                                     getColor("primary")));
    }

    if (m_forgotPasswordButton) {
        m_forgotPasswordButton->setStyleSheet(QString(
                                                  "QPushButton {"
                                                  "  color: %1;"
                                                  "  font-size: 12px;"
                                                  "  text-decoration: underline;"
                                                  "  border: none;"
                                                  "  padding: 5px;"
                                                  "}"
                                                  "QPushButton:hover {"
                                                  "  color: %2;"
                                                  "}"
                                                  ).arg(getColor("text-secondary"), getColor("primary")));
    }
}

void LoginPage::onPageShow()
{
    BaseWidget::onPageShow();
    qDebug() << "LoginPage 页面显示";

    //安全检查，防止 UI 未初始化时访问
    if (!m_layoutSetupDone || !m_usernameEdit) {
        qWarning() << "LoginPage UI 未初始化完成，跳过自动登录逻辑";
        return;
    }

    // 只在这个地方统一处理自动登录逻辑
    if (m_userSession->isLoggedIn()) {
        // 检查内存中的登录状态（比如从其他页面返回）
        UserData user = m_userSession->currentUser();
        emit loginSuccess(user);
    } else if (m_userSession->autoLoginFromSavedSession()) {
        // 尝试从本地文件恢复登录状态（比如程序重启后）
        UserData user = m_userSession->currentUser();
        emit loginSuccess(user);
    } else {
        // 没有登录状态，设置焦点到输入框
        m_usernameEdit->setFocus();
    }
}

void LoginPage::onPageHide()
{
    BaseWidget::onPageHide();
    qDebug() << "LoginPage 页面隐藏";
}
