#ifndef REGISTER_PAGE_H
#define REGISTER_PAGE_H

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
#include <QFileDialog>

class RegisterPage : public BaseWidget
{
    Q_OBJECT

public:
    explicit RegisterPage(QWidget* parent = nullptr);
    ~RegisterPage() override;

    // 设置注册页面的数据（如果需要预填充）
    void setRegisterData(const QString& username = "", const QString& email = "");

signals:
    // 页面导航信号
    void registerSuccess(const UserData& user);
    void navigateToLogin();
    void navigateToMain();

protected:
    void initUI() override;
    void setupLayout() override;
    void updateWidgetStyles() override;
    void onPageShow() override;
    void onPageHide() override;

private slots:
    void onRegisterButtonClicked();
    void onLoginButtonClicked();
    void onTermsAgreedChanged();

    // UserSession 信号处理
    void onRegisterSuccess(const QString& username);
    void onRegisterFailed(const QString& error);
    void onUserChanged(const UserData& user);

    void onAvatarSelected(); // 处理头像选择

private:
    void setupRegisterForm();
    void setupButtons();
    void setupFooter();
    void connectSignals();
    void validateInput();
    void setUIState(bool enabled);

    // UI控件
    QGroupBox* m_registerGroupBox{nullptr};
    QFormLayout* m_formLayout{nullptr};
    QLineEdit* m_usernameEdit{nullptr};
    QLineEdit* m_passwordEdit{nullptr};
    QLineEdit* m_confirmPasswordEdit{nullptr};
    QLineEdit* m_emailEdit{nullptr};
    QCheckBox* m_termsAgreementCheckBox{nullptr};

    // 按钮
    QPushButton* m_registerButton{nullptr};
    QPushButton* m_loginButton{nullptr};
    QPushButton* m_termsButton{nullptr};

    // 标签
    QLabel* m_titleLabel{nullptr};

    // 服务对象
    UserSession* m_userSession{nullptr};

    // 标记是否在初始化中
    bool m_initializing{false};
    bool m_layoutSetupDone{false};
    bool m_signalsConnected{false};
    bool m_processingRegistration{false};

    QString m_avatarPath; // 用于临时存储用户选择的头像路径
    QLabel* m_avatarLabel; // 用于显示头像的控件
    QPushButton* m_avatarButton; // 选择头像的按钮
};

#endif // REGISTER_PAGE_H
