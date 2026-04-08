#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "base/base_widget.h"
#include "user_session.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QFrame>

class LoginPage : public BaseWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget* parent = nullptr);
    ~LoginPage() override;

    // 设置登录页面的数据（如果需要预填充）
    void setLoginData(const QString& username = "", bool rememberMe = false);

signals:
    // 页面导航信号
    void loginSuccess(const UserData& user);
    void navigateToRegister();
    void navigateToMain();

protected:
    void initUI() override;
    void setupLayout() override;
    void updateWidgetStyles() override;
    void onPageShow() override;
    void onPageHide() override;

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onForgotPasswordClicked();

    // UserSession 信号处理
    void onLoginSuccess(const UserData& user);
    void onLoginFailed(const QString& error);
    //void onUserChanged(const UserData& user);
    void onLogoutSuccess(const bool flage);

private:
    void setupLoginForm();
    void setupButtons();
    void setupFooter();
    void connectSignals();
    void validateInput();
    void setUIState(bool enabled);

    // UI控件
    QGroupBox* m_loginGroupBox{nullptr};
    QFormLayout* m_formLayout{nullptr};
    QLineEdit* m_usernameEdit{nullptr};
    QLineEdit* m_passwordEdit{nullptr};
    QCheckBox* m_rememberMeCheckBox{nullptr};

    // 按钮
    QPushButton* m_loginButton{nullptr};
    QPushButton* m_registerButton{nullptr};
    QPushButton* m_forgotPasswordButton{nullptr};

    // 标签
    QLabel* m_titleLabel{nullptr};

    // 服务对象
    UserSession* m_userSession{nullptr};

    // 标记是否在初始化中
    bool m_initializing{false};
    bool m_layoutSetupDone{false};

    UserData m_lastLoggedInUser;  // 记录最后登录的用户
    bool m_processingLogin{false};
    bool m_signalsConnected{false};  //标记信号是否已连接

    bool logoutFlage{false};    //标记是否已经退出
};

#endif // LOGIN_PAGE_H
