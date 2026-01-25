// search_base.cpp
#include "search_base.h"
#include <QKeyEvent>
#include <QShortcut>
#include <QTimer>

// 私有数据类
class SearchBase::SearchBasePrivate
{
public:
    // 可以添加未来需要的数据
    QString lastSearchKeyword;  // 上次搜索的关键词
    bool isSearching{ false };    // 是否正在搜索
};

SearchBase::SearchBase(QWidget* parent)
    : BaseWidget(parent)
    , d(new SearchBasePrivate())
{
    initUI();
    setupConnections();
    updateStyles();
}

void SearchBase::initUI()
{
    // 创建主布局
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(8);

    // 创建搜索输入框
    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("搜索单词...");
    m_input->setClearButtonEnabled(false);  // 使用自定义清除按钮
    m_input->setObjectName("searchInput");

    // 创建搜索按钮
    m_searchButton = new QPushButton(this);
    m_searchButton->setText("搜索");
    m_searchButton->setObjectName("searchButton");
    m_searchButton->setCursor(Qt::PointingHandCursor);
    m_searchButton->setDefault(true);  // 设为默认按钮，响应回车

    // 创建清除按钮
    m_clearButton = new QPushButton(this);
    m_clearButton->setText("×");
    m_clearButton->setObjectName("clearButton");
    m_clearButton->setCursor(Qt::PointingHandCursor);
    m_clearButton->setVisible(false);  // 初始隐藏

    // 添加到布局
    m_mainLayout->addWidget(m_input, 1);  // 输入框占满剩余空间
    m_mainLayout->addWidget(m_searchButton);
    m_mainLayout->addWidget(m_clearButton);

    // 设置焦点策略
    setFocusPolicy(Qt::StrongFocus);
    m_input->setFocusPolicy(Qt::StrongFocus);

    // 设置对象名，便于样式定制
    setObjectName("SearchBaseWidget");
}

void SearchBase::setupConnections()
{
    // 输入框文本变化
    connect(m_input, &QLineEdit::textChanged,
        this, &SearchBase::onTextChanged);

    // 输入框回车键
    connect(m_input, &QLineEdit::returnPressed,
        this, &SearchBase::onReturnPressed);

    // 按钮点击
    connect(m_searchButton, &QPushButton::clicked,
        this, &SearchBase::onSearchClicked);

    connect(m_clearButton, &QPushButton::clicked,
        this, &SearchBase::onClearClicked);

    // 输入框焦点变化
    connect(m_input, &QLineEdit::editingFinished,
        this, [this]() { emit inputLostFocus(); });

    // 输入框焦点获得
    connect(m_input, &QLineEdit::selectionChanged,
        this, [this]() {
            if (m_input->hasFocus()) {
                emit inputFocused();
            }
        });

    // 添加快捷键：Ctrl+F 聚焦到搜索框
    QShortcut* focusShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(focusShortcut, &QShortcut::activated,
        this, &SearchBase::setFocusToInput);

    // 添加快捷键：Esc 清除输入
    QShortcut* clearShortcut = new QShortcut(QKeySequence("Esc"), this);
    connect(clearShortcut, &QShortcut::activated,
        this, &SearchBase::clearInput);
}

void SearchBase::onThemeChanged()
{
    BaseWidget::onThemeChanged();
    updateStyles();
}

void SearchBase::updateStyles()
{
    QString borderColor = getColor("border");
    QString surfaceColor = getColor("surface");
    QString textColor = getColor("text-primary");
    QString placeholderColor = getColor("text-secondary");
    QString primaryColor = getColor("primary");

    // 输入框样式
    QString inputStyle = QString(
        "QLineEdit {"
        "  border: 2px solid %1;"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "  font-size: 14px;"
        "  color: %2;"
        "  background-color: %3;"
        "  selection-background-color: %4;"
        "}"
        "QLineEdit:focus {"
        "  border-color: %4;"
        "  outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "  color: %5;"
        "  font-style: italic;"
        "}"
    ).arg(borderColor, textColor, surfaceColor, primaryColor, placeholderColor);

    m_input->setStyleSheet(inputStyle);

    // 搜索按钮样式
    QString searchButtonStyle = QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 8px 16px;"
        "  font-size: 14px;"
        "  font-weight: 500;"
        "  min-width: 60px;"
        "}"
        "QPushButton:hover {"
        "  background-color: %2;"
        "}"
        "QPushButton:pressed {"
        "  background-color: %3;"
        "}"
        "QPushButton:disabled {"
        "  background-color: %4;"
        "  color: %5;"
        "}"
    ).arg(primaryColor,
        getColor("primary-light"),
        getColor("primary-dark"),
        getColor("surface"),
        getColor("text-disabled"));

    m_searchButton->setStyleSheet(searchButtonStyle);

    // 清除按钮样式
    QString clearButtonStyle = QString(
        "QPushButton {"
        "  color: %1;"
        "  background-color: transparent;"
        "  border: 2px solid transparent;"
        "  border-radius: 6px;"
        "  padding: 8px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  min-width: 30px;"
        "  min-height: 30px;"
        "}"
        "QPushButton:hover {"
        "  background-color: %2;"
        "  color: %3;"
        "}"
    ).arg(placeholderColor,
        getColor("surface-dark"),
        getColor("text-primary"));

    m_clearButton->setStyleSheet(clearButtonStyle);

    // 整个组件的样式
    setStyleSheet(QString(
        "SearchBase {"
        "  background-color: transparent;"
        "}"
    ));
}

void SearchBase::updateClearButtonVisibility()
{
    bool hasText = !m_input->text().trimmed().isEmpty();
    m_clearButton->setVisible(hasText && m_showClearButton);
}

void SearchBase::onTextChanged(const QString& text)
{
    updateClearButtonVisibility();
    emit keywordChanged(text.trimmed());
}

void SearchBase::onSearchClicked()
{
    QString keyword = m_input->text().trimmed();
    if (!keyword.isEmpty()) {
        d->lastSearchKeyword = keyword;
        emit searchRequested(keyword);
    }
}

void SearchBase::onClearClicked()
{
    clearInput();
    m_input->setFocus();
}

void SearchBase::onReturnPressed()
{
    onSearchClicked();
}

QString SearchBase::keyword() const
{
    return m_input->text().trimmed();
}

void SearchBase::setKeyword(const QString& keyword)
{
    if (m_input->text() != keyword) {
        m_input->setText(keyword);
        updateClearButtonVisibility();
    }
}

QString SearchBase::placeholder() const
{
    return m_input->placeholderText();
}

void SearchBase::setPlaceholder(const QString& placeholder)
{
    m_input->setPlaceholderText(placeholder);
}

void SearchBase::setShowClearButton(bool show)
{
    if (m_showClearButton != show) {
        m_showClearButton = show;
        updateClearButtonVisibility();
    }
}

void SearchBase::setShowSearchButton(bool show)
{
    if (m_showSearchButton != show) {
        m_showSearchButton = show;
        m_searchButton->setVisible(show);
    }
}

void SearchBase::setFocusToInput()
{
    m_input->setFocus();
    m_input->selectAll();  // 选中所有文本，方便直接输入
}

void SearchBase::doSearch()
{
    onSearchClicked();
}

void SearchBase::clearInput()
{
    m_input->clear();
    updateClearButtonVisibility();
    emit keywordChanged("");
}

void SearchBase::keyPressEvent(QKeyEvent* event)
{
    // 处理键盘事件
    switch (event->key()) {
    case Qt::Key_Escape:
        if (m_input->hasFocus() && !m_input->text().isEmpty()) {
            clearInput();
            event->accept();
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (m_input->hasFocus()) {
            onReturnPressed();
            event->accept();
        }
        break;
    default:
        BaseWidget::keyPressEvent(event);
    }
}