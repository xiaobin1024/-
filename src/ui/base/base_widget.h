// base_widget.h - 第2版（添加样式管理）
#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QMap>
#include<QFormLayout>
#include<QPushButton>
#include<QLineEdit>
#include<QLabel>
#include<QGroupBox>
#include<QTimer>

// 界面状态枚举
enum class UIState {
    Normal,
    Loading,
    Error
};

// 界面样式主题
enum class UITheme {
    Light,
    Dark
};

//页面事件类型
enum class PageEvent{
    Show,       //页面显示
    Hide,       //页面隐藏
    Activated,  //页面激活
    Deactivated,//页面失活
    DataLoaded, //数据加载完成
    Error       //页面错误
};

//系统功能类型
enum class SystemFunction{
    ThemeToggle,    //切换主题
    Logout,         //退出登录
    DeleteAccount,  //注销账号
    Settings,       //设置
    About,          //关于
    Feeback         //反馈
};

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidget(QWidget* parent = nullptr);
    virtual ~BaseWidget();

    // ============ 页面生命周期管理 ============
    virtual void showPage();
    virtual void hidePage();
    virtual void activatePage();
    virtual void deactivatePage();
    virtual void setPageData(const QVariant& data);

    // ============ 消息管理 ============
    void showMessage(const QString& message, bool isError = false,int duration=0);
    void clearMessage();
    void showLoading(const QString& message = "加载中...");
    void hideLoading();

   // ============ 样式管理 ============
    void setUIState(UIState state);
    UIState uiState() const { return m_uiState; }
    void setUITheme(UITheme theme);
    UITheme uiTheme() const { return m_uiTheme; }
    //获取主布局
    QVBoxLayout* mainLayout(){return m_mainLayout;}
    //颜色管理
    QString getColor(const QString& role) const;

    // ============ 系统功能 ============
    void requestSystemFunction(SystemFunction function, const QVariant& data = QVariant());

signals:
    //页面事件信号
    void pageEvent(PageEvent event, const QVariant& data = QVariant());
    // 页面跳转请求
    void navigateRequested(const QString& pageName, const QVariant& data = QVariant());
    // 系统功能请求
    void systemFunctionRequested(SystemFunction function, const QVariant& data = QVariant());

    // 用户操作信号
    void logoutRequested();
    void deleteAccountRequested();
    void themeToggleRequested();
protected:
    // ============ 布局相关 ============
    virtual void initUI();
    virtual void setupLayout();

    // ============ 控件创建辅助 ============
    QPushButton* createPrimaryButton(const QString& text, const QString& objectName = "");
    QPushButton* createSecondaryButton(const QString& text, const QString& objectName = "");
    QLineEdit* createLineEdit(const QString& placeholder = "", const QString& objectName = "");
    QLabel* createLabel(const QString& text, const QString& type = "normal", const QString& objectName = "");
    QGroupBox* createGroupBox(const QString& title, const QString& objectName = "");

    // ============ 样式更新 ============
    virtual void updateWidgetStyles();
    void updateLabelStyle(QLabel* label, const QString& type = "normal");
    void updateGroupBoxStyle(QGroupBox* groupBox);

    // ============ 事件处理 ============
    virtual void onPageShow();
    virtual void onPageHide();
    virtual void onPageActivated();
    virtual void onPageDeactivated();
    virtual void onPageDataChanged(const QVariant& data);

    virtual void onThemeChanged();
    virtual void onStateChanged();

    // 错误处理
    virtual void onError(const QString& errorMessage, bool show = true);

    // 重写QWidget事件
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

protected:
    // 布局
    QVBoxLayout* m_mainLayout{nullptr};

    // 样式相关
    UIState m_uiState{UIState::Normal};
    UITheme m_uiTheme{UITheme::Light};
    QMap<QString, QString> m_colors;

    // 页面数据
    QVariant m_pageData;

    // 页面状态
    bool m_pageVisible{false};
    bool m_pageActivated{false};

    // 控件存储（用于批量更新样式）
    QList<QPushButton*> m_primaryButtons;
    QList<QPushButton*> m_secondaryButtons;
    QList<QLineEdit*> m_lineEdits;
    QList<QLabel*> m_labels;
    QList<QGroupBox*> m_groupBoxes;
private:
    // 颜色管理
    void loadColors();

    //样式统一格式辅助函数
    QString createMessageStyle(const QString& type) const;
    QString createLoadingWidgetStyle() const;
    QString createProgressBarStyle() const;
    QString createLoadingLabelStyle() const;
    QString createButtonStyle(const QString& type) const;
    QString createLineEditStyle() const;

    // 消息管理
    void setupMessageTimer(int duration);

    QList<QLabel*> m_messageLabels;     // 存储所有消息标签
    QTimer* m_messageTimer{nullptr};    // 用于消息自动清除
    QString m_currentMessage;

};

#endif // BASE_WIDGET_H
