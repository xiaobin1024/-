#include "vocabulary_page.h"
#include "word_data.h"
#include "word/interactive_wordcard.h"
#include"system/theme_manager.h"
#include <QDebug>

VocabularyPage::VocabularyPage(QWidget *parent)
    : BaseWidget(parent),
    m_wordCollect(WordCollect::instance()),
    m_wordVocabulary(WordVocabulary::instance()),
    m_exportManager(ExportManager::instance())
{
    qDebug() << "VocabularyPage 创建";
    initialize();

    // 连接 WordCollect 的信号到槽函数
    // connect(m_wordCollect, &WordCollect::collectListSuccess, this, &VocabularyPage::onCollectListSuccess);
    // connect(m_wordCollect, &WordCollect::collectListFailed, this, &VocabularyPage::onCollectListFailed);
}

VocabularyPage::~VocabularyPage()
{
    qDebug() << "VocabularyPage 销毁";
}

void VocabularyPage::initUI()
{
    qDebug() << "VocabularyPage::initUI()开始";
    BaseWidget::initUI(); // 这里 BaseWidget 应该已经初始化了 m_mainLayout (QVBoxLayout)
    setObjectName("VocabularyPage");
    setWindowTitle("我的生词本");


    qDebug() << "VocabularyPage::initUI() - 结束";
}

void VocabularyPage::setupLayout()
{
    qDebug() << "VocabularyPage::setupLayout() 开始";

    if (!m_mainLayout) {
        qCritical() << "主布局未初始化";
        return;
    }

    // 我们创建一个横向布局来管理顶部的两个按钮
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0); // 紧凑布局

    // 1. 初始化/确保按钮存在
    // 注意：m_backButton 和 m_refreshButton 应该在 initUI 或构造函数中 new 过
    if (!m_backButton) {
        m_backButton =  createSecondaryButton("","backButton");
        m_backButton->setText("⬅️");
        m_backButton->setFixedSize(30, 30);
    }
    if (!m_refreshButton) {
        m_refreshButton = createSecondaryButton("","refreshButton");
        m_refreshButton->setText("🔄");
        m_refreshButton->setFixedSize(30, 30);
    }

    // 2. 将按钮加入横向布局
    topLayout->addWidget(m_backButton);      // 左边
    topLayout->addStretch(1);                // 中间：加一个弹簧，把左右两边的控件撑开
    topLayout->addWidget(m_refreshButton);   // 右边


    // --- 中间区域：滚动列表 ---
    // 创建滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 通常收藏列表不需要横向滚动
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(50, 50, 50, 50);
    m_contentLayout->setSpacing(20);


    m_scrollArea->setWidget(m_contentWidget);

    // 底部导出按钮
    QWidget* exportWidget = new QWidget(this);
    QHBoxLayout* exportLayout = new QHBoxLayout(exportWidget);
    exportLayout->setContentsMargins(0, 0, 0, 0);

    //m_exportExcelButton = new QPushButton("📊 导出Excel", this);
    m_exportExcelButton=createSecondaryButton("","exportExcelButton");
    m_exportExcelButton->setText("📊 导出Excel");

    //m_exportPdfButton = new QPushButton("📄 导出PDF", this);
    m_exportPdfButton=createSecondaryButton("","exportPdfButton");
    m_exportPdfButton->setText("📄 导出PDF");

    m_exportExcelButton->setFixedHeight(36);
    m_exportPdfButton->setFixedHeight(36);

    exportLayout->addStretch();
    exportLayout->addWidget(m_exportExcelButton);
    exportLayout->addSpacing(600);
    exportLayout->addWidget(m_exportPdfButton);
    exportLayout->addStretch();

    // 1. 添加顶部工具栏
    m_mainLayout->addWidget(topWidget);
    m_mainLayout->addWidget(m_scrollArea, 1);
    m_mainLayout->addWidget(exportWidget);

    // --- 信号连接 ---
    connect(m_refreshButton, &QPushButton::clicked, this, &VocabularyPage::requestQueryVocabularyList);
    connect(m_backButton, &QPushButton::clicked, this, &VocabularyPage::onBackButtonClicked);
    connect(m_exportExcelButton, &QPushButton::clicked, this, &VocabularyPage::onExportExcelClicked);
    connect(m_exportPdfButton, &QPushButton::clicked, this, &VocabularyPage::onExportPdfClicked);



    updateWidgetStyles();
    qDebug() << "VocabularyPage::setupLayout() 结束";

}

void VocabularyPage::requestQueryVocabularyList()
{
    // 调用 WordCollect 的查询函数
    m_currentWordList.clear();
    qDebug()<<"VocabularyPage::requestQueryVocabularyList()";
    //m_wordCollect->queryCollectList();
}


void VocabularyPage::onVocabularyListSuccess(const QList<WordData>& wordList)
{
    m_currentWordList = wordList;
    // 1. 无条件清理旧卡片
    QList<InteractiveWordCard*> oldCards = m_contentWidget->findChildren<InteractiveWordCard*>();
    for (InteractiveWordCard* card : oldCards) {
        m_contentLayout->removeWidget(card);
        delete card;
    }
    while (m_contentLayout->count() > 0) {
        delete m_contentLayout->takeAt(0);
    }

    // 2. 空列表提示
    if (wordList.isEmpty()) {
        showMessage("没有收藏记录或者单词已经显示完成", false, 2000);
        return;
    }

    // 3. 加载新卡片
    for (const WordData& wordData : wordList) {
        InteractiveWordCard* card = new InteractiveWordCard(
            wordData, m_wordCollect, m_wordVocabulary, m_contentWidget
            );
        int currentTheme = ThemeManager::instance()->currentThemeInt();
        UITheme uiTheme =(currentTheme==0)?UITheme::Light : UITheme::Dark;
        card->setUITheme(uiTheme);
        card->setFixedWidth(400);
        m_contentLayout->addWidget(card);
    }

    m_contentLayout->addStretch();
    m_contentWidget->adjustSize();
}

void VocabularyPage::onExportExcelClicked()
{
    qDebug()<<"VocabularyPage::onExportExcelClicked()";
    if (m_currentWordList.isEmpty()) {
        showMessage("当前没有可导出的生词本单词", false, 2000);
        return;
    }
    emit exportRequested(ExportFormat::Excel, m_currentWordList);
}

void VocabularyPage::onExportPdfClicked()
{
    qDebug()<<"VocabularyPage::onExportPdfClicked()";
    if (m_currentWordList.isEmpty()) {
        showMessage("当前没有可导出的生词本单词", false, 2000);
        return;
    }
    emit exportRequested(ExportFormat::PDF, m_currentWordList);
}

void VocabularyPage::updateWidgetStyles()
{
    // 根据主题设置不同的背景色
    QString bgColor;
    if (uiTheme() == UITheme::Light) {
        // 亮色主题：侧边栏使用更明显的灰白色背景
        bgColor = "#f0f0f0";
    } else {
        // 暗色主题：侧边栏使用更明显的深灰色背景
        bgColor = "#3a3a3a";
    }



    // 1. 更新滚动区域样式 (已在 setupLayout 中设置，这里可以再次确保或仅保留逻辑)
    if (m_scrollArea) {
        m_scrollArea->setStyleSheet(QString(
                                        "QScrollArea {"
                                        "  border: 2px solid %1;"
                                        "  border-radius: 8px;"
                                        "  background-color: %2;"
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
                                        ).arg(
                                            getColor("background"),
                                            getColor("surface"),
                                            getColor("secondary-light")
                                            ));
    }

    // 2. 更新内容 Widget 样式
    if (m_contentWidget) {
        m_contentWidget->setStyleSheet("background: transparent;");
    }

    // 3. 更新占位符标签样式
    if (m_placeholderLabel) {
        m_placeholderLabel->setStyleSheet(QString(
                                              "QLabel {"
                                              "  font-size: 18px;"
                                              "  color: %1;"
                                              "  background: transparent;"
                                              "}"
                                              ).arg(getColor("text-secondary")));
    }

    QString style = QString(
                        "SystemSidebar {"
                        "  background-color: %1;"
                        "  padding: 0;"
                        "  margin: 0;"
                        "}"
                        "#buttonContainer {"
                        "  background-color: transparent;"
                        "  border: none;"
                        "}"
                        ).arg(bgColor);


    if (m_exportExcelButton) {
        m_exportExcelButton->setStyleSheet(style);
    }
    if (m_exportPdfButton) {
        m_exportPdfButton->setStyleSheet(style);
    }

    BaseWidget::updateWidgetStyles(); // 调用父类刷新基础样式

    if (m_refreshButton) {
        m_refreshButton->setStyleSheet(QString(
                                           "QPushButton {"
                                           "  background-color: transparent;"  // 平时完全透明
                                           "  border: none;"
                                           "  border-radius: 20px;"
                                           "  color: %1;"                      // 图标颜色使用次要文字色（灰色）
                                           "  font-size: 28px;"
                                           "}"
                                           "QPushButton:hover {"
                                           "  background-color: %2;"           // 悬浮时显示浅灰色背景
                                           "  color: %3;"                      // 图标变为主色
                                           "}"
                                           ).arg(getColor("text-secondary"))   // %1: 默认图标颜色
                                           .arg(getColor("surface"))          // %2: 悬浮背景 (利用 surface 颜色，亮色主题下是浅灰)
                                           .arg(getColor("primary"))          // %3: 悬浮图标颜色
                                       );
    }

    // 5. 更新返回按钮样式 (如果需要单独设置)
    if (m_backButton) {
        m_backButton->setStyleSheet(m_refreshButton->styleSheet());
    }
}

void VocabularyPage::onVocabularyListFailed(const QString& errorMessage)
{

    showMessage("显示收藏单词内容出错: " + errorMessage, true, 2000);
}

void VocabularyPage::onBackButtonClicked()
{
    qDebug()<<"VocabularyPage::onBackButtonClicked()";
    emit showMainPageRequested();
}
