#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "base/base_widget.h"
#include "word/interactive_wordcard.h"
#include "system/system_sidebar.h"
#include "search/searchhistory_widget.h"  // 添加搜索历史组件
#include"word_search.h"
#include"word_collect.h"
#include"word_vocabulary.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QLabel>

class MainPage : public BaseWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget* parent = nullptr);
    ~MainPage() override;

    // 设置搜索框的占位符文本
    void setSearchPlaceholder(const QString& placeholder);

    // 设置搜索结果
    void setSearchResults(const QStringList& results);

    // 设置当前显示的单词卡片
    void setCurrentWordCard(const WordData& wordData);

    // 获取搜索组件
    SearchHistoryWidget* getSearchWidget() const { return m_searchWidget; }

signals:
    // 搜索信号
    void searchRequested(const QString& query);

    // 从主页面导航的信号
    void navigateToLogin();
    void navigateToRegister();


protected:
    void initUI() override;
    void setupLayout() override;
    void updateWidgetStyles() override;

private slots:
    void onSearchRequested(const QString& query);
    void onClearButtonClicked();

    //单词收索结果
    void onWordSearchSuccess(const WordData& wordData);
    void onWordSearchFailed(const QString& error);


private:
    void setupHeader();
    void setupSearchSection();
    void setupContentArea();
    void setupSidebar();
    void setupConnections();

    // UI组件
    SearchHistoryWidget* m_searchWidget{nullptr};  // 使用新的搜索组件
    QWidget* m_searchContainer{nullptr};            // 搜索组件的容器
    QLabel* m_searchTitleLabel{nullptr};
    QScrollArea* m_contentScrollArea{nullptr};
    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    SystemSidebar* m_sidebar{nullptr};

    // 当前显示的单词卡片
    InteractiveWordCard* m_currentWordCard{nullptr};

    // 服务对象
    UserSession* m_userSession{nullptr};
    WordSearch* m_wordSearch{nullptr};
    WordCollect* m_wordCollect{nullptr};
    WordVocabulary* m_wordVocabulary{nullptr};
};

#endif // MAIN_PAGE_H
