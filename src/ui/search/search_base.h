#ifndef SEARCH_BASE_H
#define SEARCH_BASE_H

/**
 * @brief 基础搜索组件
 *
 * 提供基本的搜索功能，包含：
 * 1. 搜索输入框
 * 2. 搜索按钮
 * 3. 清除按钮
 * 4. 基本的样式和主题支持
 *
 * 继承自 BaseWidget，支持亮/暗主题切换
 * 使用信号机制与父组件通信
 */

#include"base/base_widget.h"
class SearchBase:public BaseWidget
{
    Q_OBJECT
public:
    explicit SearchBase(QWidget* parent = nullptr);

    // 获取和设置搜索关键词
    QString keyword() const;
    void setKeyword(const QString& keyword);

    // 获取输入框占位符文本
    QString placeholder() const;
    void setPlaceholder(const QString& placeholder);

    // 控制按钮显示
    void setShowClearButton(bool show);
    void setShowSearchButton(bool show);

    // 获取输入框和按钮控件（用于特殊定制）
    QLineEdit* inputField() const { return m_input; }
    QPushButton* searchButton() const { return m_searchButton; }
    QPushButton* clearButton() const { return m_clearButton; }

    // 设置焦点到输入框
    void setFocusToInput();
signals:
    /**
     * @brief 搜索请求信号
     * @param keyword 搜索关键词
     *
     * 在以下情况触发：
     * 1. 点击搜索按钮
     * 2. 按下回车键
     * 3. 外部调用 doSearch()
     */
    void searchRequested(const QString& keyword);

    /**
     * @brief 关键词变化信号
     * @param keyword 当前关键词
     *
     * 当输入框文本发生变化时触发
     */
    void keywordChanged(const QString& keyword);

    /**
     * @brief 输入框获得焦点信号
     */
    void inputFocused();

    /**
     * @brief 输入框失去焦点信号
     */
    void inputLostFocus();
public slots:
    // 执行搜索（触发 searchRequested 信号）
    void doSearch();

    // 清空输入框
    void clearInput();
protected:
    // 重写主题变化处理
    void onThemeChanged() override;

    // 重写键盘事件，支持快捷键
    void keyPressEvent(QKeyEvent* event) override;
private slots:
    // 处理输入框文本变化
    void onTextChanged(const QString& text);

    // 处理搜索按钮点击
    void onSearchClicked();

    // 处理清除按钮点击
    void onClearClicked();

    // 处理回车键按下
    void onReturnPressed();

    //添加事件过滤器
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUI();
    void setupConnections();
    void updateStyles();
    void updateClearButtonVisibility();

    // UI 组件
    QLineEdit* m_input{nullptr};          // 搜索输入框
    QPushButton* m_searchButton{nullptr}; // 搜索按钮
    QPushButton* m_clearButton{nullptr};  // 清除按钮

    // 布局
    //QHBoxLayout* m_mainLayout{nullptr};

    // 配置
    bool m_showClearButton{true};    // 是否显示清除按钮
    bool m_showSearchButton{true};   // 是否显示搜索按钮

    // 私有数据
    class SearchBasePrivate;
    SearchBasePrivate* d;
};

#endif // SEARCH_BASE_H
