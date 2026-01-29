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

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidget(QWidget* parent = nullptr);

    // 基本功能
    void showMessage(const QString& message, bool isError = false,int duration=0);
    void clearMessage();
    void showLoading(const QString& message = "加载中...");
    void hideLoading();

    // 样式管理
    void setUIState(UIState state);
    UIState uiState() const { return m_uiState; }

    void setUITheme(UITheme theme);
    UITheme uiTheme() const { return m_uiTheme; }

     QVBoxLayout* m_mainLayout{nullptr};
      QString getColor(const QString& role) const;

protected:
    // 布局辅助函数
    QVBoxLayout* createMainLayout();
    QHBoxLayout* createButtonLayout();
    QFormLayout* createFormLayout();

    // 控件创建函数
    QPushButton* createPrimaryButton(const QString& text);
    QPushButton* createSecondaryButton(const QString& text);
    QLineEdit* createLineEdit(const QString& placeholder = "");
    QLabel* createLabel(const QString& text, const QString& type = "normal");
    // 创建标准化的 GroupBox
    QGroupBox* createGroupBox(const QString& title);
    void updateGroupBoxStyle(QGroupBox* groupBox);  //更新GroupBox

    void updateLabelStyle(QLabel* label, const QString& type = "normal");   //更新Label

    // 样式辅助函数
    void setupMessageTimer(int duration);

    void updateWidgetStyles();

protected slots:
    virtual void onThemeChanged();
    virtual void onStateChanged();

protected:
    // 样式相关
    UIState m_uiState{UIState::Normal};
    UITheme m_uiTheme{UITheme::Light};
    QMap<QString, QString> m_colors;


    QString m_currentMessage;

private:
    void loadColors();

    //样式统一格式辅助函数
    QString createMessageStyle(const QString& type) const;
    QString createLoadingWidgetStyle() const;
    QString createProgressBarStyle() const;
    QString createLoadingLabelStyle() const;
    QString createButtonStyle(const QString& type) const;
    QString createLineEditStyle() const;

private:

    QList<QLabel*> m_messageLabels;  // 存储所有消息标签
    QTimer* m_messageTimer{nullptr};  // 用于消息自动清除

    // 存储已创建的控件
    QList<QGroupBox*> m_createdGroupBoxes;
    QList<QLabel*> m_createdLabels;


};

#endif // BASE_WIDGET_H
