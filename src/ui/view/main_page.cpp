#include "main_page.h"
#include "user_session.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QLabel>
#include <QIcon>
#include <QRegularExpressionValidator>
#include <QDebug>

MainPage::MainPage(QWidget* parent)
    : BaseWidget(parent)
    , m_userSession(UserSession::instance())
    ,m_wordSearch(WordSearch::instance())
    ,m_wordCollect(WordCollect::instance())
    ,m_wordVocabulary(WordVocabulary::instance())

{
    qDebug() << "MainPage 创建";
    initialize();
    setupConnections();
}

MainPage::~MainPage()
{
    qDebug() << "MainPage 销毁";
}

void MainPage::initUI()
{
    BaseWidget::initUI();
    setObjectName("MainPage");
    setWindowTitle("主页面 - 网络词典");
}

void MainPage::setupLayout()
{
    qDebug()<<"MainPage::setupLayout()";

    // 创建主布局
    auto* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建主要内容区域
    auto* contentArea = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // 设置头部
    setupHeader();

    // 设置搜索区域
    setupSearchSection();

    // 设置内容区域
    setupContentArea();

    m_searchContainer->setStyleSheet("#m_searchContainer { background: transparent; border: none; }");

    m_searchContainer->layout()->setContentsMargins(0, 0, 0, 0);

    // 添加主要内容到布局
    contentLayout->addWidget(m_searchTitleLabel);
    contentLayout->addWidget(m_searchContainer);  // 添加搜索容器
    contentLayout->addWidget(m_contentScrollArea,1);
    contentLayout->addStretch();

    // 设置侧边栏
    setupSidebar();

    // 将内容区域和侧边栏添加到主布局
    mainLayout->addWidget(contentArea,1);
    mainLayout->addWidget(m_sidebar);

    // 将主布局设置到页面
    m_mainLayout->addLayout(mainLayout);
}

void MainPage::setupHeader()
{
    // 标题标签
    m_searchTitleLabel = createLabel("网络词典", "title", "searchTitleLabel");
    m_searchTitleLabel->setAlignment(Qt::AlignCenter);
    m_searchTitleLabel->setStyleSheet(QString(
                                          "QLabel {"
                                          "  font-size: 28px;"
                                          "  font-weight: bold;"
                                          "  color: %1;"
                                          "  margin: 20px 0 20px 0;"
                                          "  padding: 10px;"
                                          "}"
                                          ).arg(getColor("text-primary")));
}

void MainPage::setupSearchSection()
{
    // 创建搜索组件
    m_searchWidget = new SearchHistoryWidget(this);  // 使用 this 作为父对象
    m_searchWidget->setObjectName("searchWidget");
    m_searchWidget->setMinimumHeight(50);
    //m_searchWidget->setMaximumWidth(500);
    m_searchWidget->setMinimumWidth(200);
    m_searchWidget->setPlaceholder("请输入要搜索的单词...");

    // 设置输入验证器（可选）
    QRegularExpression regex("^[a-zA-Z0-9_\\s]+$");
    auto* validator = new QRegularExpressionValidator(regex, this);
    m_searchWidget->inputField()->setValidator(validator);


    // 创建搜索组件的容器
    m_searchContainer = new QWidget(this);
    QHBoxLayout* searchContainerLayout = new QHBoxLayout(m_searchContainer);
    searchContainerLayout->setContentsMargins(20,0,20,0);
    searchContainerLayout->setSpacing(0);

    // 添加搜索组件到容器
    searchContainerLayout->addWidget(m_searchWidget);

    // 设置容器样式
    // 修改 m_searchContainer 的样式表
    m_searchContainer->setStyleSheet(QString(
                                         "QWidget {"
                                         "  background-color: transparent;"  // 关键：设为透明
                                         "  border: none;"                   // 关键：移除边框
                                         "  border-radius: 8px;"
                                         "  padding: 0px;"
                                         "}"
                                         ).arg(getColor("surface"), getColor("border")));
    // 确保容器本身的大小策略允许它扩展
    m_searchContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void MainPage::setupContentArea()
{
    // 创建滚动区域
    m_contentScrollArea = new QScrollArea(this);
    m_contentScrollArea->setWidgetResizable(true);
    m_contentScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_contentScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_contentScrollArea->setMinimumHeight(500);  // 设置最小高度
    m_contentScrollArea->setStyleSheet(QString(
                                           "QScrollArea {"
                                           "  border: 2px solid %1;"  // 增加边框宽度
                                           "  border-radius: 8px;"    // 添加圆角
                                           "  background-color: %2;"  // 背景色
                                           "  margin: 20px;"          // 外边距
                                           "}"
                                           "QScrollBar:vertical {"
                                           "  background: %2;"
                                           "  width: 12px;"
                                           "  border-radius: 6px;"
                                           "}"
                                           "QScrollBar::handle:vertical {"
                                           "  background: %3;"
                                           "  border-radius: 6px;"
                                           "  min-height: 20px;"
                                           "}"
                                           ).arg(getColor("background"), getColor("surface"), getColor("secondary-light")));


    // 创建内容区域
    m_contentWidget = new QWidget();
    m_contentWidget->setStyleSheet("QWidget { background-color: transparent; }"); // 设置为透明
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(50, 50, 50, 50);
    m_contentLayout->setSpacing(20);

    // 设置滚动区域的内容
    m_contentScrollArea->setWidget(m_contentWidget);

    // 添加欢迎信息（可选）
    auto* welcomeLabel = createLabel("欢迎使用网络词典！", "subtitle", "welcomeLabel");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(QString(
                                    "QLabel {"
                                    "  font-size: 18px;"
                                    "  color: %1;"
                                    "  margin: 50px 0;"
                                    "  padding: 20px;"
                                    "}"
                                    ).arg(getColor("text-secondary")));

    m_contentLayout->addWidget(welcomeLabel);
    m_contentLayout->addStretch();

}

void MainPage::setupSidebar()
{
    // 创建侧边栏
    m_sidebar = new SystemSidebar(this);

    // 设置侧边栏宽度
    m_sidebar->setFixedWidth(50);

    // 设置用户会话
    m_sidebar->setUserSession(UserSession::instance());

    // 连接侧边栏的系统功能信号
    connect(m_sidebar, &SystemSidebar::themeToggleRequested,
            this, &MainPage::onThemeChanged);
    connect(m_sidebar, &SystemSidebar::deleteAccountRequested,
            this, &MainPage::navigateToLogin);
}

void MainPage::setupConnections()
{
    qDebug()<<"MainPage::setupConnections()";
    // 连接搜索相关的信号

    connect(m_searchWidget, &SearchHistoryWidget::keywordChanged,
            this, &MainPage::onKeywordChanged);

    connect(m_searchWidget, &SearchHistoryWidget::searchRequested,
            this, &MainPage::onSearchRequested);
    connect(m_searchWidget, &SearchHistoryWidget::historyItemClicked,
            this, &MainPage::onSearchRequested);

    //登录，注销相关信号
    connect(m_userSession, &UserSession::logoutSuccess,
            this, &MainPage::navigateToLogin);
    connect(m_userSession,&UserSession::unregisterSuccess,
            this,&MainPage::navigateToLogin);

    // 连接 WordSearch 信号
    connect(m_wordSearch, &WordSearch::searchSuccess,
            this, &MainPage::onWordSearchSuccess);
    connect(m_wordSearch, &WordSearch::searchFailed,
            this, &MainPage::onWordSearchFailed);

    // 设置 UserSession 到 WordSearch
    m_wordSearch->setUserSession(m_userSession);
    // 设置 UserSession 到 WordCollect
    m_wordCollect->setUserSession(m_userSession);
    // 设置 UserSession 到 WordVocabulary
    m_wordVocabulary->setUserSession(m_userSession);
}

void MainPage::onSearchRequested(const QString& query)
{
    if (!query.isEmpty()) {

        m_wordSearch->searchWord(query);
    }
}

void MainPage::onWordSearchSuccess(const WordData& wordData)
{
    setCurrentWordCard(wordData);
}

void MainPage::onWordSearchFailed(const QString& error)
{
    showMessage("搜索失败: " + error, true, 2000);

    // 显示错误信息
    auto* errorLabel = createLabel("搜索失败: " + error, "normal", "searchErrorLabel");
    errorLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(getColor("error")));

    // 清除现有内容并显示错误
    while (m_contentLayout->count() > 1) {
        QLayoutItem* item = m_contentLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    m_contentLayout->insertWidget(0, errorLabel);
}

void MainPage::onClearButtonClicked()
{
    if (m_searchWidget) {
        m_searchWidget->clearInput();
        m_searchWidget->setFocusToInput();
    }
}

void MainPage::setSearchPlaceholder(const QString& placeholder)
{
    if (m_searchWidget) {
        m_searchWidget->setPlaceholder(placeholder);
    }
}

void MainPage::setSearchResults(const QStringList& results)
{
    // 清除现有的搜索结果
    while (m_contentLayout->count() > 1) { // 保留stretch
        QLayoutItem* item = m_contentLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (results.isEmpty()) {
        auto* noResultsLabel = createLabel("没有找到匹配的结果", "normal", "noResultsLabel");
        noResultsLabel->setAlignment(Qt::AlignCenter);
        m_contentLayout->insertWidget(0, noResultsLabel);
        return;
    }

    // 添加搜索结果
    for (const QString& result : results) {
        auto* resultLabel = createLabel(result, "normal", "resultLabel");
        m_contentLayout->insertWidget(0, resultLabel);
    }
}

void MainPage::setCurrentWordCard(const WordData& wordData)
{
    // 清除现有的单词卡片
    if (m_currentWordCard) {
        m_contentLayout->removeWidget(m_currentWordCard);
        m_currentWordCard->deleteLater();
        m_currentWordCard = nullptr;
    }

    // 创建新的单词卡片
    m_currentWordCard = new InteractiveWordCard(wordData,m_wordCollect,m_wordVocabulary, this);
    m_currentWordCard->setFixedWidth(600);

    // 添加到内容布局
    m_contentLayout->insertWidget(0, m_currentWordCard);

}

void MainPage::updateWidgetStyles()
{
    BaseWidget::updateWidgetStyles();

    // 更新搜索容器样式
    if (m_searchContainer) {
        m_searchContainer->setStyleSheet(QString(
                                             "QWidget {"
                                             "  background-color: %1;"
                                             "  border: 1px solid %2;"
                                             "  border-radius: 8px;"
                                             "  padding: 5px;"
                                             "}"
                                             ).arg(getColor("surface"), getColor("border")));
    }

    // 更新内容滚动区域样式
    if (m_contentScrollArea) {
        m_contentScrollArea->setStyleSheet(QString(
                                               "QScrollArea {"
                                               "  border: 2px solid %1;"  // 增加边框宽度
                                               "  border-radius: 8px;"    // 添加圆角
                                               "  background-color: %2;"  // 背景色
                                               "  margin: 20px;"          // 外边距
                                               "}"
                                               "QScrollBar:vertical {"
                                               "  background: %2;"
                                               "  width: 12px;"
                                               "  border-radius: 6px;"
                                               "}"
                                               "QScrollBar::handle:vertical {"
                                               "  background: %3;"
                                               "  border-radius: 6px;"
                                               "  min-height: 20px;"
                                               "}"
                                               ).arg(getColor("background"), getColor("surface"), getColor("secondary-light")));
    }

    // 更新标题样式
    if (m_searchTitleLabel) {
        m_searchTitleLabel->setStyleSheet(QString(
                                              "QLabel {"
                                              "  font-size: 28px;"
                                              "  font-weight: bold;"
                                              "  color: %1;"
                                              "  margin: 20px 0 20px 0;"
                                              "  padding: 10px;"
                                              "}"
                                              ).arg(getColor("text-primary")));
    }
}

void MainPage::onKeywordChanged(const QString& keyword)
{
    // 只有当关键词变为空字符串时（即点击了清理按钮），才删除当前卡片
    if (keyword.isEmpty()) {
        if (m_currentWordCard) {
            // 1. 从布局中移除
            if (m_contentLayout) {
                m_contentLayout->removeWidget(m_currentWordCard);
            }

            // 2. 安排删除 (使用 deleteLater 保证安全)
            m_currentWordCard->deleteLater();
            m_currentWordCard = nullptr;
        }
    }
}
