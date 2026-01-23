// interactive_wordcard.cpp
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

InteractiveWordCard::InteractiveWordCard(QWidget* parent)
    : WordCard(parent)
    , d(new InteractiveWordCardPrivate())
{
    initInteractiveUI();
    setupConnections();
}

InteractiveWordCard::InteractiveWordCard(const WordData& data, QWidget* parent)
    : WordCard(data, parent)
    , d(new InteractiveWordCardPrivate())
{
    initInteractiveUI();
    setupConnections();
}

void InteractiveWordCard::initInteractiveUI()
{
    // 创建按钮容器
    d->buttonContainer = new QWidget(this);
    d->buttonLayout = new QHBoxLayout(d->buttonContainer);
    d->buttonLayout->setContentsMargins(0, 0, 0, 0);
    d->buttonLayout->setSpacing(4);

    // 使用BaseWidget的函数创建按钮
    m_favoriteButton = new QPushButton(d->buttonContainer);
    m_favoriteButton->setFixedSize(24, 24);
    m_favoriteButton->setFlat(true);
    m_favoriteButton->setToolTip("收藏");
    m_favoriteButton->setCursor(Qt::PointingHandCursor);

    m_pronunciationButton = new QPushButton(d->buttonContainer);
    m_pronunciationButton->setFixedSize(24, 24);
    m_pronunciationButton->setFlat(true);
    m_pronunciationButton->setToolTip("发音");
    m_pronunciationButton->setCursor(Qt::PointingHandCursor);

    m_addToVocabularyButton = new QPushButton(d->buttonContainer);
    m_addToVocabularyButton->setFixedSize(24, 24);
    m_addToVocabularyButton->setFlat(true);
    m_addToVocabularyButton->setToolTip("添加到生词本");
    m_addToVocabularyButton->setCursor(Qt::PointingHandCursor);

    m_moreActionsButton = new QPushButton(d->buttonContainer);
    m_moreActionsButton->setFixedSize(24, 24);
    m_moreActionsButton->setFlat(true);
    m_moreActionsButton->setToolTip("更多操作");
    m_moreActionsButton->setCursor(Qt::PointingHandCursor);

    // 将按钮添加到布局
    d->buttonLayout->addStretch();
    d->buttonLayout->addWidget(m_favoriteButton);
    d->buttonLayout->addWidget(m_pronunciationButton);
    d->buttonLayout->addWidget(m_addToVocabularyButton);
    d->buttonLayout->addWidget(m_moreActionsButton);

    // 创建更多操作菜单
    createMoreActionsMenu();

    // 将按钮容器添加到主布局顶部
    m_mainLayout->insertWidget(0, d->buttonContainer);

    // 更新按钮样式
    updateButtonStyles();

    // 控制按钮可见性
    m_favoriteButton->setVisible(m_showFavoriteButton);
    m_pronunciationButton->setVisible(m_showPronunciationButton);
    m_addToVocabularyButton->setVisible(m_showAddToVocabularyButton);
    m_moreActionsButton->setVisible(m_showMoreActionsButton);
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
}

void InteractiveWordCard::onThemeChanged()
{
    WordCard::onThemeChanged();
    updateButtonStyles();
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

void InteractiveWordCard::setShowFavoriteButton(bool show)
{
    if (m_showFavoriteButton != show) {
        m_showFavoriteButton = show;
        m_favoriteButton->setVisible(show);
    }
}

void InteractiveWordCard::setShowPronunciationButton(bool show)
{
    if (m_showPronunciationButton != show) {
        m_showPronunciationButton = show;
        m_pronunciationButton->setVisible(show);
    }
}

void InteractiveWordCard::setShowAddToVocabularyButton(bool show)
{
    if (m_showAddToVocabularyButton != show) {
        m_showAddToVocabularyButton = show;
        m_addToVocabularyButton->setVisible(show);
    }
}

void InteractiveWordCard::setShowMoreActionsButton(bool show)
{
    if (m_showMoreActionsButton != show) {
        m_showMoreActionsButton = show;
        m_moreActionsButton->setVisible(show);
    }
}

void InteractiveWordCard::onFavoriteButtonClicked()
{
    m_isFavorite = !m_isFavorite;
    updateButtonStyles();

    // 发出信号
    emit favoriteToggled(getWordData().word, m_isFavorite);

    // 显示反馈
    if (m_isFavorite) {
        showMessage("已收藏", false, 1500);
    }
    else {
        showMessage("已取消收藏", false, 1500);
    }
}

void InteractiveWordCard::onPronunciationButtonClicked()
{
    if (!getWordData().word.isEmpty()) {
        emit pronunciationRequested(getWordData().word);
        showMessage("播放发音", false, 1000);
    }
}

void InteractiveWordCard::onAddToVocabularyButtonClicked()
{
    if (getWordData().isValid()) {
        if (m_isAddedToVocabulary) {
            emit removeFromVocabularyRequested(getWordData().word);
            m_isAddedToVocabulary = false;
            showMessage("已从生词本移除", false, 1500);
        }
        else {
            emit addToVocabularyRequested(getWordData());
            m_isAddedToVocabulary = true;
            showMessage("已添加到生词本", false, 1500);
        }
        updateButtonStyles();
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
    // 收藏按钮样式
    if (m_isFavorite) {
        m_favoriteButton->setStyleSheet(QString(
            "QPushButton {"
            "  border: none;"
            "  background-color: transparent;"
            "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEyIDIxLjM1TDEwLjU1IDIwLjAzQzYuMDQgMTUuNzYgMyAxMi43OSAzIDkuNUMzIDUuOTEgNS45MSAzIDkuNSAzQzExLjI0IDMgMTIuOTggMy45NCAxNCA1LjI5QzE1LjAyIDMuOTQgMTYuNzYgMyAxOC41IDNDMjIuMDkgMyAyNSA1LjkxIDI1IDkuNUMyNSAxMi43OSAyMS45NiAxNS43NiAxNy40NSAyMC4wNEwxNiAyMS4zNVoiIGZpbGw9IiNFRjQ0NDQiIGZpbGwtcnVsZT0iZXZlbm9kZCIgY2xpcC1ydWxlPSJldmVub2RkIi8+Cjwvc3ZnPg==);"
            "}"
            "QPushButton:hover {"
            "  background-color: #FEE2E2;"
            "  border-radius: 4px;"
            "}"
        ));
    }
    else {
        m_favoriteButton->setStyleSheet(QString(
            "QPushButton {"
            "  border: none;"
            "  background-color: transparent;"
            "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTE2LjUgM0MxNC43NiAzIDEzLjA5IDMuOTcgMTIgNS42OSAxMC45MSAzLjk3IDkuMjQgMyA3LjUgMyA0LjQyIDMgMiA1LjQyIDIgOC41IDIgMTUuNTUgOS41IDIwLjI5IDEyIDIyLjAxIDE0LjUgMjAuMjkgMjIgMTUuNTUgMjIgOC41IDIyIDUuNDIgMTkuNTggMyAxNi41IDNaIiBzdHJva2U9IiUxIiBzdHJva2Utd2lkdGg9IjIiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIgc3Ryb2tlLWxpbmVqb2luPSJyb3VuZCIvPgo8L3N2Zz4K);"
            "}"
            "QPushButton:hover {"
            "  background-color: %1;"
            "  border-radius: 4px;"
            "}"
        ).arg(getColor("primary-light") + "20"));

        // 替换颜色
        QString style = m_favoriteButton->styleSheet();
        style.replace("%1", getColor("text-secondary"));
        m_favoriteButton->setStyleSheet(style);
    }

    // 发音按钮样式
    m_pronunciationButton->setStyleSheet(QString(
        "QPushButton {"
        "  border: none;"
        "  background-color: transparent;"
        "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTMgOXY2aDNsNS41IDVWNEw2IDlIM1pNMTYuNSAxMkMxNi41IDEwLjIzIDE1LjQ4IDguNzEgMTQgOC4xOFYxNS44M0MxNS40OCAxNS4yOSAxNi41IDEzLjc3IDE2LjUgMTJabTAtM0MxNi41IDcuMjQgMTQuNyA1IDEyLjUgNUgxMXYxNGgxLjVDMTQuNyAxOSAxNi41IDE2Ljc2IDE2LjUgMTJaIiBmaWxsPSIlMSIvPgo8L3N2Zz4K);"
        "}"
        "QPushButton:hover {"
        "  background-color: %1;"
        "  border-radius: 4px;"
        "}"
    ).arg(getColor("primary-light") + "20").replace("%1", getColor("text-primary")));

    // 添加到生词本按钮样式
    if (m_isAddedToVocabulary) {
        m_addToVocabularyButton->setStyleSheet(QString(
            "QPushButton {"
            "  border: none;"
            "  background-color: transparent;"
            "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTUgMTMuMTFsMi41NCAyLjQ5QTkgOSAwIDAgMCAxMiAyMWM1IDAgOS0zLjU4IDktOHMtNC05LTktOGE5IDkgMCAwMC04LjkxIDRINDFsMyAzSDNsMy0zSDN2M3oiIGZpbGw9IiMxMEE5ODEiLz4KPC9zdmc+);"
            "}"
            "QPushButton:hover {"
            "  background-color: #D1FAE5;"
            "  border-radius: 4px;"
            "}"
        ));
        m_addToVocabularyButton->setToolTip("从生词本移除");
    }
    else {
        m_addToVocabularyButton->setStyleSheet(QString(
            "QPushButton {"
            "  border: none;"
            "  background-color: transparent;"
            "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEyIDV2MTRtLTctN2gxNFoiIHN0cm9rZT0iJTEiIHN0cm9rZS13aWR0aD0iMiIgc3Ryb2tlLWxpbmVjYXA9InJvdW5kIiBzdHJva2UtbGluZWpvaW49InJvdW5kIi8+Cjwvc3ZnPg==);"
            "}"
            "QPushButton:hover {"
            "  background-color: %1;"
            "  border-radius: 4px;"
            "}"
        ).arg(getColor("primary-light") + "20").replace("%1", getColor("text-primary")));
        m_addToVocabularyButton->setToolTip("添加到生词本");
    }

    // 更多操作按钮样式
    m_moreActionsButton->setStyleSheet(QString(
        "QPushButton {"
        "  border: none;"
        "  background-color: transparent;"
        "  qproperty-icon: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjQiIGhlaWdodD0iMjQiIHZpZXdCb3g9IjAgMCAyNCAyNCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEyIDEzYTEgMSAwIDEwMC0yIDEgMSAwIDAwMCAyem0tNy0xYTEgMSAwIDEwMC0yIDEgMSAwIDAwMCAyem0xNCAwYTEgMSAwIDEwMC0yIDEgMSAwIDAwMCAyeCIgZmlsbD0iJTEiLz4KPC9zdmc+);"
        "}"
        "QPushButton:hover {"
        "  background-color: %1;"
        "  border-radius: 4px;"
        "}"
    ).arg(getColor("primary-light") + "20").replace("%1", getColor("text-primary")));
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
        QApplication::clipboard()->setText(getWordData().word);
        showMessage("已复制到剪贴板", false, 1000);
        });
}