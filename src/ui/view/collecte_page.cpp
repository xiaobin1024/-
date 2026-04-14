#include "collecte_page.h"
#include "word_data.h"
#include "word/interactive_wordcard.h"
#include"system/theme_manager.h"
#include <QDebug>

CollectePage::CollectePage(QWidget *parent)
    : BaseWidget(parent),
    m_wordCollect(WordCollect::instance()),
    m_wordVocabulary(WordVocabulary::instance())
{
    qDebug() << "CollectePage 创建";
    initialize();

    // 连接 WordCollect 的信号到槽函数
    connect(WordCollect::instance(), &WordCollect::collectListSuccess, this, &CollectePage::onCollectListSuccess);
    connect(WordCollect::instance(), &WordCollect::collectListFailed, this, &CollectePage::onCollectListFailed);
}


CollectePage::~CollectePage()
{
    qDebug() << "CollectePage 销毁";
}

void CollectePage::initUI()
{
    qDebug() << "CollectePage::initUI()开始";
    BaseWidget::initUI();
    setObjectName("CollectePage");
    setWindowTitle("我的收藏");



    // 初始化占位符标签
    m_placeholderLabel = new QLabel("暂无收藏记录", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("QLabel { font-size: 18px; color: gray; }");
     qDebug() << "CollectePage::initUI() - 结束";
}

void CollectePage::setupLayout()
{
    qDebug() << "CollectePage::setupLayout() 开始";


    if (!m_mainLayout) {
        qCritical() << "主布局未初始化";
        return;
    }

    // 1. 创建滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet(QString(
                                    "QScrollArea { border: none; background: transparent; }"
                                    "QScrollBar:vertical { background: %1; width: 12px; border-radius: 6px; }"
                                    "QScrollBar::handle:vertical { background: %2; border-radius: 6px; min-height: 20px; }"
                                    ).arg(getColor("surface"), getColor("secondary-light")));

    // 2. 创建内容 Widget 和 Layout
    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(50, 50, 50, 50);
    m_contentLayout->setSpacing(20);
    m_contentWidget->setStyleSheet("background: transparent;");

    // 默认添加占位符
    m_contentLayout->addWidget(m_placeholderLabel);
    m_contentLayout->addStretch();

    // 3. 设置滚动区域内容
    m_scrollArea->setWidget(m_contentWidget);

    // 4. 设置主布局
    m_mainLayout->addWidget(m_scrollArea);

    // === 添加刷新按钮 ===
    m_refreshButton = new QPushButton("刷新", this);
    m_refreshButton->setFixedHeight(40);
    m_refreshButton->setStyleSheet(QString(
                                       "QPushButton {"
                                       "  background-color: %1;"
                                       "  color: %2;"
                                       "  border: 1px solid %3;"
                                       "  border-radius: 6px;"
                                       "  padding: 0 15px;"
                                       "  font-size: 14px;"
                                       "  font-weight: 500;"
                                       "}"
                                       "QPushButton:hover {"
                                       "  background-color: %4;"
                                       "}"
                                       "QPushButton:pressed {"
                                       "  background-color: %5;"
                                       "}"
                                       ).arg(
                                           getColor("primary"),        // 背景颜色
                                           getColor("text-primary"),   // 文字颜色
                                           getColor("border"),         // 边框颜色
                                           getColor("primary-light"),  // 悬停颜色
                                           getColor("primary-dark")    // 按下颜色
                                           ));

    // 将按钮添加到主布局底部
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_refreshButton, 0, Qt::AlignBottom);
    m_mainLayout->addSpacing(20);

    // 连接按钮点击信号到查询函数
    connect(m_refreshButton, &QPushButton::clicked, this, &CollectePage::requestQueryCollectList);
    qDebug() << "CollectePage::setupLayout() 结束";

}

void CollectePage::requestQueryCollectList()
{
    // 调用 WordCollect 的查询函数
    WordCollect::instance()->queryCollectList();

    // 显示加载状态
    m_placeholderLabel->setText("正在加载收藏列表...");
    m_placeholderLabel->setVisible(true);
}

void CollectePage::onCollectListSuccess(const QList<WordData>& wordList)
{
    // 清除旧内容（除了占位符和 Stretch）
    // QLayoutItem* item;
    // while ((item = m_contentLayout->takeAt(0)) != nullptr) {
    //     if (item->widget()) {
    //         item->widget()->deleteLater();
    //     }
    //     delete item;
    // }

    if (wordList.isEmpty()) {
        m_contentLayout->addWidget(m_placeholderLabel);
        m_contentLayout->addStretch();
        m_placeholderLabel->setText("暂无收藏记录");
        return;
    }

    // 遍历数据，创建单词卡片
    for (const WordData& wordData : wordList) {
        InteractiveWordCard* card = new InteractiveWordCard(wordData, m_wordCollect, m_wordVocabulary, this);

        // 【关键】应用当前主题
        int currentTheme = ThemeManager::instance()->currentThemeInt();
        UITheme uiTheme = (currentTheme == 0) ? UITheme::Light : UITheme::Dark;
        card->setUITheme(uiTheme);

        card->setFixedWidth(400); // 限制卡片宽度
        m_contentLayout->addWidget(card);
    }
    m_contentLayout->addStretch();
}

void CollectePage::updateWidgetStyles()
{
    BaseWidget::updateWidgetStyles(); // 调用父类刷新基础样式

    // 1. 更新滚动区域样式 (对应 m_scrollArea)
    // 包括边框、背景色和滚动条样式
    if (m_scrollArea) {
        m_scrollArea->setStyleSheet(QString(
                                        "QScrollArea {"
                                        "  border: 2px solid %1;"     // 边框颜色，使用背景色作为边框参考
                                        "  border-radius: 8px;"       // 圆角
                                        "  background-color: %2;"     // 背景色，使用 surface 颜色
                                        "  margin: 20px;"             // 外边距
                                        "}"
                                        "QScrollBar:vertical {"
                                        "  background: %2;"          // 滚动条轨道背景
                                        "  width: 12px;"              // 滚动条宽度
                                        "  border-radius: 6px;"       // 圆角
                                        "}"
                                        "QScrollBar::handle:vertical {"
                                        "  background: %3;"           // 滚动条滑块颜色
                                        "  border-radius: 6px;"       // 圆角
                                        "  min-height: 20px;"         // 最小高度
                                        "}"
                                        ).arg(
                                            getColor("background"),      // %1: 边框颜色
                                            getColor("surface"),         // %2: 表面背景色 (用于轨道和边框内背景)
                                            getColor("secondary-light")  // %3: 滑块颜色
                                            ));
    }

    // 2. 更新内容 Widget 样式 (对应 m_contentWidget)
    // 确保内容区域背景透明，以显示滚动区域的背景
    if (m_contentWidget) {
        m_contentWidget->setStyleSheet("background: transparent;"); // 关键：设为透明
    }

    // 3. 更新占位符标签样式 (对应 m_placeholderLabel)
    // 当没有收藏内容时显示的文字样式
    if (m_placeholderLabel) {
        m_placeholderLabel->setStyleSheet(QString(
                                              "QLabel {"
                                              "  font-size: 18px;"
                                              "  color: %1;"                // 文字颜色，使用次级文本颜色
                                              "  background: transparent;"  // 背景透明
                                              "}"
                                              ).arg(getColor("text-secondary")));
    }

    // 4. 更新刷新按钮样式
    if (m_refreshButton) {
        m_refreshButton->setStyleSheet(QString(
                                           "QPushButton {"
                                           "  background-color: %1;"
                                           "  color: %2;"
                                           "  border: 1px solid %3;"
                                           "  border-radius: 6px;"
                                           "  padding: 0 15px;"
                                           "  font-size: 14px;"
                                           "  font-weight: 500;"
                                           "}"
                                           "QPushButton:hover {"
                                           "  background-color: %4;"
                                           "}"
                                           "QPushButton:pressed {"
                                           "  background-color: %5;"
                                           "}"
                                           ).arg(
                                               getColor("primary"),        // 背景颜色
                                               getColor("text-primary"),   // 文字颜色
                                               getColor("border"),         // 边框颜色
                                               getColor("primary-light"),  // 悬停颜色
                                               getColor("primary-dark")    // 按下颜色
                                               ));
    }
}

void CollectePage::onCollectListFailed(const QString& errorMessage)
{
    // 清除旧内容（除了占位符和 Stretch）
    // QLayoutItem* item;
    // while ((item = m_contentLayout->takeAt(0)) != nullptr) {
    //     if (item->widget()) {
    //         item->widget()->deleteLater();
    //     }
    //     delete item;
    // }

    // // 显示错误信息
    // m_placeholderLabel->setText(errorMessage);
    // m_contentLayout->addWidget(m_placeholderLabel);
    // m_contentLayout->addStretch();

     showMessage("收藏单词已经接收完成: " + errorMessage, true, 2000);
}
