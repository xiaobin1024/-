#ifndef SYSTEM_SIDEBAR_H
#define SYSTEM_SIDEBAR_H

#include "base/base_widget.h"
#include"user_session.h"
#include"message_dispatcher.h"
#include <QPropertyAnimation>

class QPushButton;
class QVBoxLayout;
class QLabel;

/**
 * @brief 系统功能侧边栏
 *
 * 继承自BaseWidget，提供：
 * 1. 主题切换按钮（通过ThemeManager全局切换）
 * 2. 退出登录按钮
 * 3. 注销账号按钮
 * 4. 展开/收起动画功能
 * 5. 自适应主题的系统样式
 */
class SystemSidebar : public BaseWidget
{
    Q_OBJECT
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)

public:
    explicit SystemSidebar(QWidget* parent = nullptr);
    ~SystemSidebar();

    // 新增：设置外部模块
    void setUserSession(UserSession* session);
    void setMessageDispatcher(MessageDispatcher* dispatcher);


    // ==================== 状态控制 ====================

    /**
     * @brief 获取当前展开状态
     * @return true=展开，false=收起
     */
    bool isExpanded() const { return m_expanded; }

    /**
     * @brief 设置展开状态
     * @param expanded 展开状态
     *
     * 会触发展开/收起动画
     */
    void setExpanded(bool expanded);

    /**
     * @brief 切换展开/收起状态
     */
    void toggle();

    /**
     * @brief 获取当前实际宽度
     * @return 当前宽度
     */
    int currentWidth() const { return m_currentWidth; }

    /**
     * @brief 设置当前宽度（供动画使用）
     * @param width 宽度
     */
    void setCurrentWidth(int width);

    // 尺寸常量
    static constexpr int EXPANDED_WIDTH = 180;
    static constexpr int COLLAPSED_WIDTH = 50;

signals:
    /**
     * @brief 展开状态变化信号
     * @param expanded 新的展开状态
     */
    void expandedChanged(bool expanded);

    /**
     * @brief 退出登录请求信号
     *
     * 用户点击退出登录按钮时发射
     */
    void logoutRequested();

    /**
     * @brief 注销账号请求信号
     *
     * 用户点击注销账号按钮时发射
     */
    void deleteAccountRequested();

    void loginRequested(const QString& username, const QString& password);

    void registerRequested(const QString& username, const QString& password);


    // 注意：themeToggleRequested信号已从BaseWidget继承

protected:
    // 重写BaseWidget的初始化方法
    void setupLayout() override;

    // 重写BaseWidget的主题变化处理
    void onThemeChanged() override;

    // 重写BaseWidget的页面显示/隐藏处理
    void onPageShow() override;
    void onPageHide() override;

    // 重写绘制事件，实现圆角和阴影
    void paintEvent(QPaintEvent* event) override;

    // 重写尺寸事件，调整按钮容器大小
    void resizeEvent(QResizeEvent* event) override;

public slots:
    void onUserLoggedIn(const UserData& user);
    void onUserLoggedOut();
    void onUserLoginFailed(const QString& error);
    void onUserLogoutSuccess();
    void onUserUnregisterSuccess(const QString& username, int userId);
    void onUserUnregisterFailed(const QString& error);
    void onUserChanged(const UserData& user);

private slots:
    // 按钮点击处理
    void handleThemeButtonClicked();
    void handleLogoutButtonClicked();
    void handleDeleteAccountButtonClicked();

    // 动画处理
    void onAnimationFinished();

    // 样式更新
    void updateSidebarStyle();

private:
    // 初始化UI组件
    void createButtons();
    void setupAnimations();

    // 启动展开/收起动画
    void startExpandAnimation(bool expand);

    MessageDispatcher* m_messageDispatcher;
    UserSession* m_userSession;


    void updateUserButtonsState();

    // 状态变量
    bool m_expanded{true};           // 当前展开状态
    int m_currentWidth{EXPANDED_WIDTH}; // 当前宽度

    // 布局
    QVBoxLayout* m_buttonLayout{nullptr};   // 按钮垂直布局
    QWidget* m_buttonContainer{nullptr};    // 按钮容器

    // 按钮组件
    QPushButton* m_themeButton{nullptr};      // 主题切换按钮
    QPushButton* m_logoutButton{nullptr};     // 退出登录按钮
    QPushButton* m_deleteButton{nullptr};     // 注销账号按钮
    QPushButton* m_toggleButton{nullptr};     // 展开/收起按钮

    // 动画
    QPropertyAnimation* m_widthAnimation{nullptr};  // 宽度动画
    QPropertyAnimation* m_opacityAnimation{nullptr}; // 透明度动画


    // 新增布局成员变量 - 修复关键问题
    QWidget* m_topButtons = nullptr;
    QVBoxLayout* m_topLayout = nullptr;
    QWidget* m_bottomContainer = nullptr;
    QVBoxLayout* m_bottomLayout = nullptr;
    QSpacerItem* m_spacer = nullptr;

    // 样式缓存
    QString m_currentStyleSheet;
};

#endif // SYSTEM_SIDEBAR_H
