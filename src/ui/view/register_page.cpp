#include "register_page.h"
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

RegisterPage::RegisterPage(QWidget* parent)
    : BaseWidget(parent)
    , m_userSession(UserSession::instance())
    , m_layoutSetupDone(false)
{
    qDebug() << "RegisterPage 创建";
    initialize();

    // 初始化 UserSession
    if (!m_userSession->isInitialized()) {
        m_userSession->initialize();
    }

    connectSignals();
}

RegisterPage::~RegisterPage()
{
    qDebug() << "RegisterPage 销毁";
    if (m_userSession) {
        m_userSession->disconnect(this);
    }
}

void RegisterPage::initUI()
{
    BaseWidget::initUI();
    setObjectName("RegisterPage");

    // 设置页面标题
    setWindowTitle("注册 - 网络词典");
}

void RegisterPage::setupLayout()
{
    if (m_layoutSetupDone) {
        return; // 防止重复初始化
    }

    qDebug()<<"RegisterPage::setupLayout()";

    // 创建主布局容器
    auto* mainContainer = new QWidget(this);
    auto* mainContainerLayout = new QVBoxLayout(mainContainer);
    mainContainerLayout->setContentsMargins(0, 0, 0, 0);
    mainContainerLayout->setSpacing(20);

    // 标题
    m_titleLabel = createLabel("用户注册", "title", "titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString(
                                    "QLabel {"
                                    "  font-size: 24px;"
                                    "  font-weight: bold;"
                                    "  color: %1;"
                                    "  margin: 20px 0 30px 0;"
                                    "}"
                                    ).arg(getColor("text-primary")));

    // 注册表单组
    m_registerGroupBox = createGroupBox("注册信息", "registerGroupBox");
    m_formLayout = new QFormLayout(m_registerGroupBox);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_formLayout->setLabelAlignment(Qt::AlignRight);
    m_formLayout->setFormAlignment(Qt::AlignCenter);
    m_formLayout->setSpacing(15);
    m_formLayout->setContentsMargins(20, 20, 20, 20);

    setupRegisterForm();
    setupButtons();
    setupFooter();

    // 添加到主容器
    mainContainerLayout->addWidget(m_titleLabel, 0, Qt::AlignTop);
    mainContainerLayout->addWidget(m_registerGroupBox, 0, Qt::AlignCenter);
    mainContainerLayout->addStretch();

    // 将主容器添加到基类布局
    m_mainLayout->addWidget(mainContainer, 0, Qt::AlignCenter);

    m_layoutSetupDone = true;
}

void RegisterPage::setupRegisterForm()
{
    // 用户名输入
    m_usernameEdit = createLineEdit("请输入用户名", "usernameEdit");
    m_usernameEdit->setObjectName("usernameEdit");
    m_usernameEdit->setMinimumWidth(300);
    m_usernameEdit->setMaximumWidth(400);

    // 设置用户名输入验证器
    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    auto* usernameValidator = new QRegularExpressionValidator(usernameRegex, this);
    m_usernameEdit->setValidator(usernameValidator);

    // 密码输入
    m_passwordEdit = createLineEdit("请输入密码", "passwordEdit");
    m_passwordEdit->setObjectName("passwordEdit");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumWidth(300);
    m_passwordEdit->setMaximumWidth(400);

    // 确认密码输入
    m_confirmPasswordEdit = createLineEdit("请再次输入密码", "confirmPasswordEdit");
    m_confirmPasswordEdit->setObjectName("confirmPasswordEdit");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setMinimumWidth(300);
    m_confirmPasswordEdit->setMaximumWidth(400);

    // 邮箱输入
    m_emailEdit = createLineEdit("请输入邮箱地址", "emailEdit");
    m_emailEdit->setObjectName("emailEdit");
    m_emailEdit->setMinimumWidth(300);
    m_emailEdit->setMaximumWidth(400);

    // 设置邮箱输入验证器
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    auto* emailValidator = new QRegularExpressionValidator(emailRegex, this);
    m_emailEdit->setValidator(emailValidator);

    // 同意条款复选框
    m_termsAgreementCheckBox = new QCheckBox("我已阅读并同意用户协议和隐私政策", this);
    m_termsAgreementCheckBox->setObjectName("termsAgreementCheckBox");
    m_termsAgreementCheckBox->setChecked(false);
    m_termsAgreementCheckBox->setStyleSheet(QString(
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
                                                "  border: 2px solid %5;"
                                                "}"
                                                ).arg(getColor("text-primary"),
                                                     getColor("border"),
                                                     getColor("surface"),
                                                     getColor("primary"),
                                                     getColor("primary")));

    // 添加到表单布局
    m_formLayout->addRow("用户名:", m_usernameEdit);
    m_formLayout->addRow("密码:", m_passwordEdit);
    m_formLayout->addRow("确认密码:", m_confirmPasswordEdit);
    m_formLayout->addRow("邮箱:", m_emailEdit);
    m_formLayout->addRow(m_termsAgreementCheckBox);

    // 连接输入验证
    connect(m_usernameEdit, &QLineEdit::textChanged, this, &RegisterPage::validateInput);
    connect(m_passwordEdit, &QLineEdit::textChanged, this, &RegisterPage::validateInput);
    connect(m_confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterPage::validateInput);
    connect(m_emailEdit, &QLineEdit::textChanged, this, &RegisterPage::validateInput);
    connect(m_termsAgreementCheckBox, &QCheckBox::stateChanged, this, &RegisterPage::onTermsAgreedChanged);
}

void RegisterPage::setupButtons()
{
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->setAlignment(Qt::AlignCenter);

    // 注册按钮
    m_registerButton = createPrimaryButton("注册", "registerButton");
    m_registerButton->setMinimumWidth(120);
    m_registerButton->setMinimumHeight(40);
    m_registerButton->setEnabled(false); // 初始禁用，直到输入有效

    // 登录按钮
    m_loginButton = createSecondaryButton("已有账户？去登录", "loginButton");
    m_loginButton->setMinimumWidth(120);
    m_loginButton->setMinimumHeight(40);

    buttonLayout->addWidget(m_registerButton);
    buttonLayout->addWidget(m_loginButton);

    m_formLayout->addRow(buttonLayout);
}

void RegisterPage::setupFooter()
{
    auto* footerLayout = new QHBoxLayout();
    footerLayout->setAlignment(Qt::AlignCenter);

    // 查看条款按钮
    m_termsButton = createSecondaryButton("查看用户协议", "termsButton");
    m_termsButton->setFlat(true);
    m_termsButton->setStyleSheet(QString(
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

    footerLayout->addWidget(m_termsButton);

    m_formLayout->addRow(footerLayout);

    // 连接按钮信号
    connect(m_registerButton, &QPushButton::clicked, this, &RegisterPage::onRegisterButtonClicked);
    connect(m_loginButton, &QPushButton::clicked, this, &RegisterPage::onLoginButtonClicked);
    connect(m_termsButton, &QPushButton::clicked, this, &RegisterPage::onTermsAgreedChanged);
}

void RegisterPage::connectSignals()
{
    if (m_signalsConnected) {
        qDebug() << "RegisterPage 信号已连接，跳过重复连接";
        return;
    }

    // UserSession 信号连接
    bool connected1 = connect(m_userSession, &UserSession::registerSuccess,
                              this, &RegisterPage::onRegisterSuccess);
    bool connected2 = connect(m_userSession, &UserSession::registerFailed,
                              this, &RegisterPage::onRegisterFailed);
    bool connected3 = connect(m_userSession, &UserSession::userChanged,
                              this, &RegisterPage::onUserChanged);

    m_signalsConnected = true;

    qDebug() << "RegisterPage 信号连接状态:"
             << "registerSuccess:" << connected1
             << "registerFailed:" << connected2
             << "userChanged:" << connected3;
}

void RegisterPage::onRegisterButtonClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirmPassword = m_confirmPasswordEdit->text();
    QString email = m_emailEdit->text().trimmed();
    bool termsAgreed = m_termsAgreementCheckBox->isChecked();

    // 验证输入
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || email.isEmpty()) {
        showMessage("请填写所有必填项", true, 2000);
        return;
    }

    if (password != confirmPassword) {
        showMessage("两次输入的密码不一致", true, 2000);
        m_confirmPasswordEdit->setFocus();
        return;
    }

    if (password.length() < 6) {
        showMessage("密码长度至少6位", true, 2000);
        m_passwordEdit->setFocus();
        return;
    }

    if (!termsAgreed) {
        showMessage("请同意用户协议和隐私政策", true, 2000);
        return;
    }

    // 验证用户名格式
    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    if (!usernameRegex.match(username).hasMatch()) {
        showMessage("用户名格式不正确（3-20个字符，字母数字下划线）", true, 2000);
        m_usernameEdit->setFocus();
        return;
    }

    // 验证邮箱格式
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        showMessage("邮箱格式不正确", true, 2000);
        m_emailEdit->setFocus();
        return;
    }

    // 禁用UI防止重复点击
    setUIState(false);
    showLoading("正在注册...");

    // 发起注册请求
    m_userSession->registerUser(username, password);
}

void RegisterPage::onLoginButtonClicked()
{
    emit navigateToLogin();
}

void RegisterPage::onTermsAgreedChanged()
{
    // 更新注册按钮状态
    validateInput();
}

void RegisterPage::onRegisterSuccess(const QString& username)
{
    if (m_processingRegistration) {
        qDebug() << "RegisterPage::onRegisterSuccess - 正在处理注册，忽略重复调用";
        return;
    }

    m_processingRegistration = true;

    qDebug() << "RegisterPage::onRegisterSuccess 被调用，用户名:" << username;

    hideLoading();
    setUIState(true);

    showMessage("注册成功！欢迎加入我们", false, 2000);

    // 获取注册成功的用户信息
    UserData user = m_userSession->currentUser();
    emit registerSuccess(user);
    qDebug() << "RegisterPage 发射 registerSuccess 信号";

    m_processingRegistration = false;

    // 注册成功后跳转到登录页面
    emit navigateToLogin();
}

void RegisterPage::onRegisterFailed(const QString& error)
{
    hideLoading();
    setUIState(true);

    showMessage("注册失败：" + error, true, 3000);

    // 清空密码字段
    m_passwordEdit->clear();
    m_confirmPasswordEdit->clear();
    m_usernameEdit->setFocus();
}

void RegisterPage::onUserChanged(const UserData& user)
{
    if (user.isLoggedIn()) {
        // 用户已登录，可能是自动登录
        // 在注册成功后会自动登录，所以这里会触发
    } else if (!user.isValid()) {
        // 用户登出或注销
        // 可以在这里做一些清理工作
    }
}

void RegisterPage::validateInput()
{
    if (!m_usernameEdit || !m_passwordEdit || !m_confirmPasswordEdit ||
        !m_emailEdit || !m_termsAgreementCheckBox || !m_registerButton) {
        return; // 控件还未创建
    }

    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirmPassword = m_confirmPasswordEdit->text();
    QString email = m_emailEdit->text().trimmed();
    bool termsAgreed = m_termsAgreementCheckBox->isChecked();

    bool valid = !username.isEmpty() && !password.isEmpty() &&
                 !confirmPassword.isEmpty() && !email.isEmpty() &&
                 password == confirmPassword &&
                 password.length() >= 6 &&
                 username.length() >= 3 && username.length() <= 20 &&
                 termsAgreed;

    m_registerButton->setEnabled(valid);
}

void RegisterPage::setUIState(bool enabled)
{
    if (m_usernameEdit) m_usernameEdit->setEnabled(enabled);
    if (m_passwordEdit) m_passwordEdit->setEnabled(enabled);
    if (m_confirmPasswordEdit) m_confirmPasswordEdit->setEnabled(enabled);
    if (m_emailEdit) m_emailEdit->setEnabled(enabled);
    if (m_termsAgreementCheckBox) m_termsAgreementCheckBox->setEnabled(enabled);

    // 只有在允许UI操作时才根据输入有效性设置注册按钮状态
    if (enabled) {
        validateInput();
    } else {
        if (m_registerButton) m_registerButton->setEnabled(false);
    }

    if (m_loginButton) m_loginButton->setEnabled(enabled);
}

void RegisterPage::setRegisterData(const QString& username, const QString& email)
{
    // 安全检查
    if (!m_usernameEdit || !m_emailEdit) {
        qWarning() << "RegisterPage UI 未初始化完成";
        return;
    }

    m_initializing = true;

    m_usernameEdit->setText(username);
    m_emailEdit->setText(email);

    if (!username.isEmpty()) {
        m_passwordEdit->setFocus();
    } else {
        m_usernameEdit->setFocus();
    }

    validateInput();

    m_initializing = false;
}

void RegisterPage::updateWidgetStyles()
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

    if (m_termsAgreementCheckBox) {
        m_termsAgreementCheckBox->setStyleSheet(QString(
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
                                                    "  border: 2px solid %5;"
                                                    "}"
                                                    ).arg(getColor("text-primary"),
                                                         getColor("border"),
                                                         getColor("surface"),
                                                         getColor("primary"),
                                                         getColor("primary")));
    }

    if (m_termsButton) {
        m_termsButton->setStyleSheet(QString(
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

void RegisterPage::onPageShow()
{
    BaseWidget::onPageShow();
    qDebug() << "RegisterPage 页面显示";

    // 安全检查，防止 UI 未初始化时访问
    if (!m_layoutSetupDone || !m_usernameEdit) {
        qWarning() << "RegisterPage UI 未初始化完成，跳过页面逻辑";
        return;
    }

    // 设置焦点到用户名输入框
    m_usernameEdit->setFocus();
}

void RegisterPage::onPageHide()
{
    BaseWidget::onPageHide();
    qDebug() << "RegisterPage 页面隐藏";
}
