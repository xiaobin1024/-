#include "interactive_wordcard.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>



// 私有数据类
class InteractiveWordCard::InteractiveWordCardPrivate
{
public:
    QWidget* buttonContainer{ nullptr };
    QHBoxLayout* buttonLayout{ nullptr };
    QMenu* moreActionsMenu{ nullptr };
};

InteractiveWordCard::InteractiveWordCard(const WordData& data,
                                         WordCollect* wordCollector,
                                         WordVocabulary *wordVocabulary,
                                         QWidget* parent)
    : WordCard(data, parent)
    , d(new InteractiveWordCardPrivate())
    , m_wordData(data)
    ,m_wordCollector(wordCollector)
    ,m_wordVocabulary(wordVocabulary)
{
    m_isFavorite = data.isCollected;
    m_isAddedToVocabulary=data.isVocabulary;

    //初始化语音引擎
    speech = new QTextToSpeech(this);
    //设置语音为英语
    speech->setLocale(QLocale(QLocale::English,QLocale::UnitedStates));

    initInteractiveUI();
    setupConnections();
}

void InteractiveWordCard::initInteractiveUI()
{
    // 创建按钮容器
    d->buttonContainer = new QWidget(this);
    d->buttonLayout = new QHBoxLayout(d->buttonContainer);
    d->buttonLayout->setContentsMargins(0, 0, 0, 0);
    d->buttonLayout->setSpacing(8);

    // 创建按钮并设置名称
    m_favoriteButton = new QPushButton(d->buttonContainer);
    m_favoriteButton->setObjectName("favoriteButton"); // 添加这一行
    m_favoriteButton->setFixedSize(30, 30);
    m_favoriteButton->setFlat(true);
    m_favoriteButton->setToolTip("收藏");
    m_favoriteButton->setCursor(Qt::PointingHandCursor);

    m_pronunciationButton = new QPushButton(d->buttonContainer);
    m_pronunciationButton->setObjectName("pronunciationButton"); // 添加这一行
    m_pronunciationButton->setFixedSize(30, 30);
    m_pronunciationButton->setFlat(true);
    m_pronunciationButton->setToolTip("发音");
    m_pronunciationButton->setCursor(Qt::PointingHandCursor);

    m_addToVocabularyButton = new QPushButton(d->buttonContainer);
    m_addToVocabularyButton->setObjectName("addToVocabularyButton"); // 添加这一行
    m_addToVocabularyButton->setFixedSize(30, 30);
    m_addToVocabularyButton->setFlat(true);
    m_addToVocabularyButton->setToolTip("添加到生词本");
    m_addToVocabularyButton->setCursor(Qt::PointingHandCursor);

    m_moreActionsButton = new QPushButton(d->buttonContainer);
    m_moreActionsButton->setObjectName("moreActionsButton"); // 添加这一行
    m_moreActionsButton->setFixedSize(30, 30);
    m_moreActionsButton->setFlat(true);
    m_moreActionsButton->setToolTip("更多操作");
    m_moreActionsButton->setCursor(Qt::PointingHandCursor);

    // 将按钮添加到布局
    d->buttonLayout->addWidget(m_favoriteButton);
    d->buttonLayout->addWidget(m_pronunciationButton);
    d->buttonLayout->addWidget(m_addToVocabularyButton);
    d->buttonLayout->addWidget(m_moreActionsButton);
    d->buttonLayout->addStretch();

    // 创建更多操作菜单
    createMoreActionsMenu();

    // 我们需要将按钮容器添加到内容之后
    m_mainLayout->addWidget(d->buttonContainer);

    d->buttonContainer->setVisible(true);

    // 更新按钮样式
    updateButtonStyles();

    // 控制按钮可见性
    m_favoriteButton->setVisible(true);
    m_pronunciationButton->setVisible(true);
    m_addToVocabularyButton->setVisible(true);
    m_moreActionsButton->setVisible(true);

    // 强制更新布局
    layout()->update();
}

void InteractiveWordCard::setupConnections()
{
    connect(m_favoriteButton, &QPushButton::clicked,
            this, &InteractiveWordCard::onFavoriteButtonClicked);

    connect(m_pronunciationButton, &QPushButton::clicked,
            this, &InteractiveWordCard::onPronunciationButtonClicked);

    connect(m_addToVocabularyButton, &QPushButton::clicked,
            this, &InteractiveWordCard::onAddToVocabularyButtonClicked);

    connect(m_moreActionsButton, &QPushButton::clicked,
            this, &InteractiveWordCard::onMoreActionsButtonClicked);

    if (m_wordCollector) {
        connect(m_wordCollector, &WordCollect::collectStatusChanged,
                this, &InteractiveWordCard::onCollectStatusChanged);
    }

    if(m_wordVocabulary){
        connect(m_wordVocabulary,&WordVocabulary::vocabularyStatusChanged,
                this,&InteractiveWordCard::onVocabularyStatusChanged);
    }
}


void InteractiveWordCard::setFavorite(bool favorite)
{
    if (m_isFavorite != favorite) {
        m_isFavorite = favorite;
        updateButtonStyles();

        // 显示反馈消息
        if (favorite) {
            showMessage("已添加到收藏", false, 1500);
        }
        else {
            showMessage("已取消收藏", false, 1500);
        }
    }
}

void InteractiveWordCard::setAddedToVocabulary(bool added)
{
    if (m_isAddedToVocabulary != added) {
        m_isAddedToVocabulary = added;
        updateButtonStyles();
    }
}

void InteractiveWordCard::onFavoriteButtonClicked()
{
    qDebug() << "收藏按钮被点击，当前状态：" << m_isFavorite;

    // 发送请求给 WordCollect
    if (m_wordCollector) {
        // 将当前 WordData 传递过去，WordCollect 会从中读取 isCollected 状态
        m_wordCollector->collectWord(m_wordData);
    }
}

void InteractiveWordCard::onCollectStatusChanged( QString &word,bool isCollected)
{
    // 收到来自服务器的最终确认，更新本地状态和UI
    if(word==m_wordData.word){
        m_isFavorite = isCollected;
        m_wordData.isCollected = isCollected; // 同步回 WordData
        updateButtonStyles();
        // 显示最终结果消息
        showMessage(isCollected ? "收藏成功" : "已取消收藏", false, 1500);
    }
}

void InteractiveWordCard::onAddToVocabularyButtonClicked()
{
    qDebug() << "生词按钮被点击，当前状态：" << m_isAddedToVocabulary;

    // 发送请求给 WordVocabulary
    if (m_wordVocabulary) {
        // 将当前 WordData WordVocabulary 会从中读取 isVocabulary 状态
        m_wordVocabulary->updateVocabularyStatus(m_wordData);
    }
}

void InteractiveWordCard::onVocabularyStatusChanged(const QString& word,bool isVocabulary)
{
    if(word==m_wordData.word){
        // 收到来自服务器的最终确认，更新本地状态和UI
        m_isAddedToVocabulary = isVocabulary;
        m_wordData.isVocabulary= isVocabulary; // 同步回 WordData
        updateButtonStyles();
        // 显示最终结果消息
        showMessage(isVocabulary ? "添加生单词成功" : "移除生单词", false, 1500);
    }
}

void InteractiveWordCard::onPronunciationButtonClicked()
{

    qDebug()<<"InteractiveWordCard::onPronunciationButtonClicked()";
    if(m_wordData.isValid()){
        qDebug()<<"m_wordData.word: "<<m_wordData.word;
        //播放单词发音
        speech->say(m_wordData.word);

    }

}



void InteractiveWordCard::onMoreActionsButtonClicked()
{
    if (d->moreActionsMenu) {
        d->moreActionsMenu->popup(m_moreActionsButton->mapToGlobal(
            QPoint(0, m_moreActionsButton->height())));
    }
}

void InteractiveWordCard::updateButtonStyles()
{
    qDebug() << "更新按钮样式";

    // 基础按钮样式
    QString baseStyle = QString(
                            "QPushButton {"
                            "  border: 1px solid %1;"
                            "  background-color: %2;"
                            "  min-width: 28px;"
                            "  min-height: 28px;"
                            "  border-radius: 4px;"
                            "  font-size: 16px;"
                            "  color: %3;"
                            "}"
                            "QPushButton:hover {"
                            "  background-color: %4;"
                            "}"
                            ).arg(getColor("border"),
                                 getColor("surface"),
                                 getColor("text-primary"),
                                 getColor("primary-light") + "20");

    // 应用样式
    m_favoriteButton->setStyleSheet(baseStyle);
    m_pronunciationButton->setStyleSheet(baseStyle);
    m_addToVocabularyButton->setStyleSheet(baseStyle);
    m_moreActionsButton->setStyleSheet(baseStyle);

    // 收藏按钮 - 使用图片
    if (m_isFavorite) {
        QIcon starBrightIcon(":/icons/star_gray.png");
        if (starBrightIcon.isNull()) {
            qWarning() << "star_bright.png 图标加载失败，使用备用图标";
            m_favoriteButton->setText("❤️");
            m_favoriteButton->setIcon(QIcon());
        } else {
            qDebug() << "star_bright.png 图标加载成功";
            m_favoriteButton->setIcon(starBrightIcon);
            m_favoriteButton->setIconSize(QSize(20, 20));
            m_favoriteButton->setText("");
        }
    } else {
        QIcon starGrayIcon(":/icons/star_gray.png");
        if (starGrayIcon.isNull()) {
            qWarning() << "star_gray.png 图标加载失败，使用备用图标";
            m_favoriteButton->setText("🤍");
            m_favoriteButton->setIcon(QIcon());
        } else {
            qDebug() << "star_gray.png 图标加载成功";
            m_favoriteButton->setIcon(starGrayIcon);
            m_favoriteButton->setIconSize(QSize(20, 20));
            m_favoriteButton->setText("");
        }
    }

    // 发音按钮 - 使用图片
    QIcon speakerIcon(":/icons/speaker.png");
    if (speakerIcon.isNull()) {
        qWarning() << "speaker.png 图标加载失败，使用备用图标";
        m_pronunciationButton->setText("🔊");
        m_pronunciationButton->setIcon(QIcon());
    } else {
        qDebug() << "speaker.png 图标加载成功";
        m_pronunciationButton->setIcon(speakerIcon);
        m_pronunciationButton->setIconSize(QSize(20, 20));
        m_pronunciationButton->setText("");
    }

    // 添加到生词本按钮
    if (m_isAddedToVocabulary) {
        QIcon colVocabIcon(":/icons/col_vocabulary.png");
        if (colVocabIcon.isNull()) {
             m_addToVocabularyButton->setText("⭐");
            m_addToVocabularyButton->setIcon(QIcon());
        } else {
            m_addToVocabularyButton->setIcon(colVocabIcon);
            m_addToVocabularyButton->setIconSize(QSize(20, 20));
            m_addToVocabularyButton->setText("");
        }
    } else {
        QIcon vocabIcon(":/icons/vocabulary.png");
        if (vocabIcon.isNull()) {
            m_addToVocabularyButton->setText("☆");
            m_addToVocabularyButton->setIcon(QIcon());
        } else {
            m_addToVocabularyButton->setIcon(vocabIcon);
            m_addToVocabularyButton->setIconSize(QSize(20, 20));
            m_addToVocabularyButton->setText("");
        }
    }

    // 更多操作按钮
    QIcon moreActionsIcon(":/icons/more_actions.png");
    if (moreActionsIcon.isNull()) {
        m_moreActionsButton->setText("⋯");
        m_moreActionsButton->setIcon(QIcon());
    } else {
        m_moreActionsButton->setIcon(moreActionsIcon);
        m_moreActionsButton->setIconSize(QSize(20, 20));
        m_moreActionsButton->setText("");
    }

    // 设置工具提示
    m_favoriteButton->setToolTip(m_isFavorite ? "取消收藏" : "收藏");
    m_pronunciationButton->setToolTip("发音");
    m_addToVocabularyButton->setToolTip(m_isAddedToVocabulary ? "从生词本移除" : "添加到生词本");
    m_moreActionsButton->setToolTip("更多操作");

    qDebug() << "按钮样式更新完成";
}
void InteractiveWordCard::createMoreActionsMenu()
{
    d->moreActionsMenu = new QMenu(this);
    d->moreActionsMenu->setStyleSheet(
        "QMenu {"
        "  background-color: white;"
        "  border: 1px solid #E2E8F0;"
        "  border-radius: 6px;"
        "  padding: 4px;"
        "}"
        "QMenu::item {"
        "  padding: 6px 12px;"
        "  border-radius: 4px;"
        "  color: #1E293B;"
        "  font-size: 13px;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #F1F5F9;"
        "}"
        );

    QAction* editAction = new QAction("编辑单词", d->moreActionsMenu);
    QAction* shareAction = new QAction("分享", d->moreActionsMenu);
    QAction* noteAction = new QAction("添加笔记", d->moreActionsMenu);
    QAction* copyAction = new QAction("复制", d->moreActionsMenu);

    d->moreActionsMenu->addAction(editAction);
    d->moreActionsMenu->addAction(shareAction);
    d->moreActionsMenu->addAction(noteAction);
    d->moreActionsMenu->addSeparator();
    d->moreActionsMenu->addAction(copyAction);

    connect(editAction, &QAction::triggered, this, [this]() {
        emit editRequested(getWordData());
    });

    connect(shareAction, &QAction::triggered, this, [this]() {
        emit shareRequested(getWordData());
    });

    connect(noteAction, &QAction::triggered, this, [this]() {
        showMessage("笔记功能开发中...", false, 1500);
    });

    connect(copyAction, &QAction::triggered, this, [this]() {
        //QApplication::clipboard()->setText(getWordData().word);
        showMessage("已复制到剪贴板", false, 1000);
    });
}

void InteractiveWordCard::onThemeChanged()
{
    WordCard::onThemeChanged();
    // 更新按钮样式
    updateButtonStyles();
    qDebug() << "InteractiveWordCard 主题切换完成";
}

void InteractiveWordCard::updateCardStyle()
{
    // 调用父类实现
    WordCard::updateCardStyle();

    // 设置按钮容器透明
     d->buttonContainer->setStyleSheet("background-color: transparent;");

    // 更新按钮样式
    updateButtonStyles();
}

void InteractiveWordCard::cardThemeChanged()
{
    InteractiveWordCard::onThemeChanged();
}
