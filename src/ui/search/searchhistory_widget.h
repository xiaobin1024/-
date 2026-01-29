#ifndef SEARCHHISTORY_WIDGET_H
#define SEARCHHISTORY_WIDGET_H

/**
 * @brief 搜索历史记录组件
 *
 * 扩展 SearchBase，添加搜索历史功能：
 * 1. 输入框获得焦点时，显示最近5条搜索记录
 * 2. 点击历史记录项，自动填充到搜索框并触发搜索
 * 3. 支持历史记录的添加、删除、保存和加载
 * 4. 支持历史记录的最大数量限制
 * 5. 历史记录持久化存储
 */
#include "search_base.h"
#include<QListWidget>
#include <QTimer>
#include<QDateTime>
#include<QMouseEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class SearchHistoryWidget:public SearchBase
{
    Q_OBJECT
public:
    explicit SearchHistoryWidget(QWidget* parent = nullptr);
    ~SearchHistoryWidget();

    // 历史记录管理
    void addHistory(const QString& keyword);
    void clearHistory();
    int historyCount() const;
    QStringList getHistory() const;

    // 配置
    void setMaxHistoryCount(int count);
    int maxHistoryCount() const;

    // 持久化
    void saveHistory(const QString& storageKey = "search_history");
    void loadHistory(const QString& storageKey = "search_history");

    // 控制历史列表显示
    void showHistory();
    void hideHistory();
    bool isHistoryVisible() const;

    // 用于测试：获取历史列表
    QListWidget* getHistoryList() const { return m_historyList; }
    QWidget* getHistoryContainer() const { return m_historyContainer; }


signals:
    // 新增信号
    void historyItemClicked(const QString& keyword);
    void historyCleared();
protected:
    // 重写键盘事件
    //void keyPressEvent(QKeyEvent* event) override;

    // 重写主题变化
    void onThemeChanged() override;
private slots:
    // 内部槽函数
    void onHistoryItemClicked(QListWidgetItem* item);
    void onInputFocused();     // 输入框获得焦点
    void onInputLostFocus();   // 输入框失去焦点
    void onSearchPerformed();  // 搜索完成
    void onClearHistoryClicked();  // 清除历史记录
private:
    void initHistoryUI();
    void setupHistoryConnections();
    void updateHistoryList();
    void updateHistoryStyle();
    void adjustHistoryPosition();
    void ensureHistoryOrder();

    // 历史记录数据结构
    struct HistoryItem {
        QString keyword;
        QDateTime timestamp;
        int searchCount{0};
    };

    // UI组件
    QListWidget* m_historyList{nullptr};
    QWidget* m_historyContainer{nullptr};
    QVBoxLayout* m_historyLayout{nullptr};
    QPushButton* m_clearHistoryButton{nullptr};

    // 历史记录存储
    QList<HistoryItem> m_historyItems;

    // 配置
    int m_maxHistoryCount{5};           // 最大历史记录数
    bool m_historyVisible{false};       // 历史列表是否可见
    bool m_autoShowHistory{true};       // 是否自动显示历史记录
    bool m_autoHideHistory{true};       // 是否自动隐藏历史记录

    // 定时器
    QTimer* m_hideTimer{nullptr};

    // 私有数据
    class SearchHistoryWidgetPrivate;
    SearchHistoryWidgetPrivate* d;
};

#endif // SEARCHHISTORY_WIDGET_H
