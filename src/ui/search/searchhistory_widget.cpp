#include "searchhistory_widget.h"
#include <QSettings>
#include <QDateTime>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QApplication>
#include <QScreen>

// 私有数据类
class SearchHistoryWidget::SearchHistoryWidgetPrivate
{
public:
        // 可以添加更多私有数据
};

SearchHistoryWidget::SearchHistoryWidget(QWidget* parent)
    : SearchBase(parent)
    , d(new SearchHistoryWidgetPrivate())
{
    qDebug() << "=== SearchHistoryWidget 构造函数开始 ===";

    initHistoryUI();
    qDebug() << "initHistoryUI() 完成";

    setupHistoryConnections();
    qDebug() << "setupHistoryConnections() 完成";

    loadHistory();
    qDebug() << "loadHistory() 完成";

    // 创建隐藏定时器
    qDebug() << "创建隐藏定时器";
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(500);
    connect(m_hideTimer, &QTimer::timeout, this, [this]() {
        QWidget* focusedWidget = QApplication::focusWidget();
        bool inputHasFocus = (focusedWidget == inputField());
        if (!inputHasFocus) {
            hideHistory();
        }
    });

    qDebug() << "=== SearchHistoryWidget 构造函数完成 ===";
}


SearchHistoryWidget::~SearchHistoryWidget()
{
    // 保存历史记录
    saveHistory();

    // 清理弹出窗口
    if (m_historyContainer) {
        m_historyContainer->deleteLater();
    }

    delete d;
}

void SearchHistoryWidget::initHistoryUI()
{
    qDebug() << "初始化搜索历史UI";

    // 【修复】安全检查
    if (!inputField()) {
        qCritical() << "SearchHistoryWidget::initHistoryUI - 输入框未初始化";
        return;
    }

    // 1. 创建独立的弹出窗口
    m_historyContainer = new QWidget(nullptr);  // 独立窗口，不设父对象
    m_historyContainer->setObjectName("historyContainer");
    m_historyContainer->setWindowFlags(
        Qt::Popup |
        Qt::FramelessWindowHint |
        Qt::NoDropShadowWindowHint
        );
    m_historyContainer->setAttribute(Qt::WA_TranslucentBackground);
    m_historyContainer->setAttribute(Qt::WA_ShowWithoutActivating);
    m_historyContainer->setAttribute(Qt::WA_DeleteOnClose, false);
    m_historyContainer->setVisible(false);

    // 2. 创建历史记录列表
    m_historyList = new QListWidget(m_historyContainer);
    m_historyList->setObjectName("historyList");
    m_historyList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_historyList->setSelectionMode(QListWidget::SingleSelection);
    m_historyList->setFocusPolicy(Qt::NoFocus);

    // 3. 创建清除历史按钮
    m_clearHistoryButton = new QPushButton(m_historyContainer);
    m_clearHistoryButton->setObjectName("clearHistoryButton");
    m_clearHistoryButton->setText("清除历史记录");
    m_clearHistoryButton->setCursor(Qt::PointingHandCursor);

    // 4. 创建弹出窗口内部的布局
    m_historyLayout = new QVBoxLayout(m_historyContainer);
    m_historyLayout->setContentsMargins(8, 8, 8, 8);
    m_historyLayout->setSpacing(4);
    m_historyLayout->addWidget(m_historyList, 1);
    m_historyLayout->addWidget(m_clearHistoryButton);

    // 5. 设置初始样式
    updateHistoryStyle();

    qDebug() << "搜索历史UI初始化完成";
    qDebug() << "历史容器父对象:" << m_historyContainer->parent();
    qDebug() << "历史容器窗口标志:" << m_historyContainer->windowFlags();
}

void SearchHistoryWidget::setupHistoryConnections()
{
    // 【修复】安全检查
    if (!inputField()) {
        qWarning() << "SearchHistoryWidget::setupHistoryConnections - 输入框未初始化";
        return;
    }

    // 使用父类SearchBase的焦点信号
    connect(this, &SearchBase::inputFocused,
            this, &SearchHistoryWidget::onInputFocused);

    connect(this, &SearchBase::inputLostFocus,
            this, &SearchHistoryWidget::onInputLostFocus);

    // 搜索信号
    connect(this, &SearchBase::searchRequested,
            this, &SearchHistoryWidget::onSearchPerformed);

    // 历史项点击
    connect(m_historyList, &QListWidget::itemClicked,
            this, &SearchHistoryWidget::onHistoryItemClicked);

    // 清除历史按钮
    connect(m_clearHistoryButton, &QPushButton::clicked,
            this, &SearchHistoryWidget::onClearHistoryClicked);

    // 监听输入框文本变化
    connect(inputField(), &QLineEdit::textChanged, this, [this](const QString& text) {
        if (text.isEmpty() && inputField()->hasFocus() && !m_historyItems.isEmpty()) {
            // 输入框为空且获得焦点，显示历史记录
            QTimer::singleShot(50, this, [this]() {
                if (inputField()->hasFocus()) {
                    showHistory();
                }
            });
        }
        else if (!text.isEmpty() && m_historyVisible) {
            // 当用户开始输入时，隐藏历史记录
            hideHistory();
        }
    });

    // 窗口激活状态变化
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget* old, QWidget* now) {
        if (m_historyVisible) {
            // 检查焦点是否还在SearchHistoryWidget内
            bool focusInWidget = false;
            QWidget* widget = now;
            while (widget) {
                if (widget == this || widget == m_historyContainer) {
                    focusInWidget = true;
                    break;
                }
                widget = widget->parentWidget();
            }

            if (!focusInWidget) {
                hideHistory();
            }
        }
    });

    // 添加检查定时器检查搜索框，显示历史记录
    QTimer* checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, [this]() {
        if (inputField()->hasFocus() &&
            m_autoShowHistory &&
            !m_historyItems.isEmpty() &&
            inputField()->text().isEmpty() &&
            !m_historyVisible) {
            showHistory();
        }
    });
    checkTimer->start(600);  // 每100ms检查一次
}

void SearchHistoryWidget::onThemeChanged()
{
    // 先调用父类的主题更新
    SearchBase::onThemeChanged();

    // 更新历史列表样式
    updateHistoryStyle();
}

void SearchHistoryWidget::updateHistoryStyle()
{
    QString backgroundColor = getColor("surface");
    QString borderColor = getColor("border");
    QString textColor = getColor("text-primary");
    QString hoverColor = getColor("primary-light") + "20";
    QString selectedColor = getColor("primary-light") + "40";

    // 历史容器样式
    QString containerStyle = QString(
                                 "QWidget {"
                                 "  background-color: %1;"
                                 "  border: 1px solid %2;"
                                 "  border-radius: 6px;"
                                 "}"
                                 ).arg(backgroundColor, borderColor);

    m_historyContainer->setStyleSheet(containerStyle);

    // 历史列表样式
    QString listStyle = QString(
                            "QListWidget {"
                            "  background-color: transparent;"
                            "  border: none;"
                            "  outline: none;"
                            "}"
                            "QListWidget::item {"
                            "  background-color: transparent;"
                            "  color: %1;"
                            "  border-radius: 4px;"
                            "  padding: 6px 8px;"
                            "}"
                            "QListWidget::item:hover {"
                            "  background-color: %2;"
                            "}"
                            "QListWidget::item:selected {"
                            "  background-color: %3;"
                            "}"
                            "QScrollBar:vertical {"
                            "  border: none;"
                            "  background-color: transparent;"
                            "  width: 6px;"
                            "}"
                            "QScrollBar::handle:vertical {"
                            "  background-color: %4;"
                            "  border-radius: 3px;"
                            "}"
                            "QScrollBar::handle:vertical:hover {"
                            "  background-color: %5;"
                            "}"
                            ).arg(textColor, hoverColor, selectedColor,
                                 getColor("border"), getColor("text-secondary"));

    m_historyList->setStyleSheet(listStyle);

    // 清除按钮样式
    QString clearButtonStyle = QString(
                                   "QPushButton {"
                                   "  color: %1;"
                                   "  background-color: transparent;"
                                   "  border: 1px solid transparent;"
                                   "  border-radius: 4px;"
                                   "  padding: 6px;"
                                   "  font-size: 12px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "  background-color: %2;"
                                   "  border-color: %3;"
                                   "}"
                                   ).arg(getColor("text-secondary"), hoverColor, getColor("border"));

    m_clearHistoryButton->setStyleSheet(clearButtonStyle);
}

void SearchHistoryWidget::onInputFocused()
{
    qDebug() << "SearchHistoryWidget: 输入框获得焦点";

    // 立即停止并重置隐藏定时器
    m_hideTimer->stop();

    if (m_autoShowHistory && !m_historyItems.isEmpty() && inputField()->text().isEmpty()) {
        // 立即显示，不使用定时器
        if (inputField()->hasFocus()) {
            showHistory();
        }
    }
}

void SearchHistoryWidget::onInputLostFocus()
{
    qDebug() << "SearchHistoryWidget: 输入框失去焦点";

    if (m_autoHideHistory) {
        // 延迟隐藏，给用户点击历史项的时间
        m_hideTimer->start(300);
    }
}

void SearchHistoryWidget::showHistory()
{
    if (m_historyItems.isEmpty() || !inputField()) {
        return;
    }

    if (!m_historyVisible) {
        // 调整位置
        adjustHistoryPosition();

        // 显示历史容器
        m_historyContainer->show();
        m_historyContainer->raise();

        m_historyVisible = true;

        // 取消隐藏定时器
        m_hideTimer->stop();

        qDebug() << "搜索历史列表显示";
    }
}

void SearchHistoryWidget::hideHistory()
{
    if (m_historyVisible) {
        m_historyContainer->hide();
        m_historyVisible = false;

        qDebug() << "搜索历史列表隐藏";
    }
}

void SearchHistoryWidget::adjustHistoryPosition()
{
    if (!m_historyContainer || !inputField()) {
        return;
    }

    // 获取输入框的全局坐标
    QPoint globalPos = inputField()->mapToGlobal(QPoint(0, inputField()->height()));

    // 获取屏幕可用区域
    QRect screenRect = screen()->availableGeometry();

    // 计算容器尺寸
    int containerWidth = inputField()->width();
    int itemHeight = 30;  // 每个历史项的高度
    int headerHeight = 40;  // 标题和按钮的高度
    int maxVisibleItems = qMin(m_historyItems.size(), 5);
    int containerHeight = maxVisibleItems * itemHeight + headerHeight;

    // 计算最佳位置
    int x = globalPos.x();
    int y = globalPos.y() + 2;  // 加2像素的间隔

    // 如果下方空间不足，显示在上方
    if (y + containerHeight > screenRect.bottom()) {
        y = inputField()->mapToGlobal(QPoint(0, 0)).y() - containerHeight - 2;
    }

    // 如果右侧超出，左移
    if (x + containerWidth > screenRect.right()) {
        x = screenRect.right() - containerWidth;
    }

    // 设置历史容器位置和大小
    m_historyContainer->setGeometry(x, y, containerWidth, containerHeight);
}

// ... 其余函数保持不变（addHistory, clearHistory, onHistoryItemClicked等）

void SearchHistoryWidget::addHistory(const QString& keyword)
{
    if (keyword.trimmed().isEmpty()) {
        return;
    }

    // 查找是否已存在相同关键词
    auto it = std::find_if(m_historyItems.begin(), m_historyItems.end(),
                           [&keyword](const HistoryItem& item) {
                               return item.keyword == keyword;
                           });

    if (it != m_historyItems.end()) {
        // 已存在，更新次数和时间戳
        it->searchCount++;
        it->timestamp = QDateTime::currentDateTime();
    } else {
        // 新记录
        HistoryItem newItem;
        newItem.keyword = keyword;
        newItem.timestamp = QDateTime::currentDateTime();
        newItem.searchCount = 1;
        m_historyItems.append(newItem);
    }

    // 确保顺序（按时间倒序）
    ensureHistoryOrder();

    // 限制数量
    if (m_historyItems.size() > m_maxHistoryCount) {
        m_historyItems = m_historyItems.mid(0, m_maxHistoryCount);
    }

    // 更新UI
    updateHistoryList();

    // 保存到设置
    saveHistory();
}

void SearchHistoryWidget::clearHistory()
{
    m_historyItems.clear();
    m_historyList->clear();
    emit historyCleared();
    saveHistory();
}

int SearchHistoryWidget::historyCount() const
{
    return m_historyItems.size();
}

QStringList SearchHistoryWidget::getHistory() const
{
    QStringList result;
    for (const auto& item : m_historyItems) {
        result.append(item.keyword);
    }
    return result;
}

void SearchHistoryWidget::setMaxHistoryCount(int count)
{
    if (count > 0 && count != m_maxHistoryCount) {
        m_maxHistoryCount = count;
        if (m_historyItems.size() > m_maxHistoryCount) {
            m_historyItems = m_historyItems.mid(0, m_maxHistoryCount);
            updateHistoryList();
        }
    }
}

int SearchHistoryWidget::maxHistoryCount() const
{
    return m_maxHistoryCount;
}

void SearchHistoryWidget::saveHistory(const QString& storageKey)
{
    qDebug() << "=== saveHistory 开始 ===";
    qDebug() << "存储键:" << storageKey;
    qDebug() << "历史记录数量:" << m_historyItems.size();

    if (m_historyItems.isEmpty()) {
        qDebug() << "历史记录为空，不保存";
        return;
    }

    QSettings settings;
    qDebug() << "QSettings 组织名:" << settings.organizationName();
    qDebug() << "QSettings 应用名:" << settings.applicationName();
    qDebug() << "QSettings 格式:" << settings.format();
    qDebug() << "QSettings 文件名:" << settings.fileName();

    QJsonArray historyArray;
    for (int i = 0; i < m_historyItems.size(); ++i) {
        const auto& item = m_historyItems[i];
        qDebug() << "处理历史记录" << i << ":";
        qDebug() << "  关键词:" << item.keyword;
        qDebug() << "  时间戳:" << item.timestamp.toString(Qt::ISODate);
        qDebug() << "  搜索次数:" << item.searchCount;

        QJsonObject obj;
        obj["keyword"] = item.keyword;
        obj["timestamp"] = item.timestamp.toString(Qt::ISODate);
        obj["count"] = item.searchCount;
        historyArray.append(obj);
    }

    QJsonDocument doc(historyArray);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    qDebug() << "生成的JSON字符串:" << jsonString;
    qDebug() << "JSON字符串长度:" << jsonString.length();

    QString key = QString("SearchHistory/%1").arg(storageKey);
    qDebug() << "保存键:" << key;

    settings.setValue(key, jsonString);
    settings.sync();  // 立即同步到磁盘

    qDebug() << "设置值后立即检查:";
    bool contains = settings.contains(key);
    qDebug() << "设置中是否包含键" << key << ":" << contains;

    if (contains) {
        QString savedValue = settings.value(key).toString();
        qDebug() << "保存的值:" << savedValue.left(100) << "...";
    }

    qDebug() << "=== saveHistory 完成 ===";
}

void SearchHistoryWidget::loadHistory(const QString& storageKey)
{
    qDebug() << "=== loadHistory 开始 ===";
    qDebug() << "存储键:" << storageKey;

    QSettings settings;
    qDebug() << "QSettings 组织名:" << settings.organizationName();
    qDebug() << "QSettings 应用名:" << settings.applicationName();
    qDebug() << "QSettings 文件名:" << settings.fileName();

    QString key = QString("SearchHistory/%1").arg(storageKey);
    qDebug() << "加载键:" << key;

    if (!settings.contains(key)) {
        qDebug() << "设置中不包含键" << key;
        qDebug() << "所有可用键:";
        QStringList allKeys = settings.allKeys();
        for (const QString& k : allKeys) {
            qDebug() << "  " << k;
        }
        return;
    }

    QString jsonString = settings.value(key).toString();
    qDebug() << "加载的JSON字符串:" << jsonString;
    qDebug() << "JSON字符串长度:" << jsonString.length();

    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "JSON解析失败或不是数组";
        return;
    }

    m_historyItems.clear();
    QJsonArray array = doc.array();
    qDebug() << "JSON数组大小:" << array.size();

    for (int i = 0; i < array.size(); ++i) {
        QJsonValue value = array.at(i);
        if (value.isObject()) {
            QJsonObject obj = value.toObject();

            HistoryItem item;
            item.keyword = obj["keyword"].toString();
            item.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
            item.searchCount = obj["count"].toInt();

            qDebug() << "解析历史记录" << i << ":";
            qDebug() << "  关键词:" << item.keyword;
            qDebug() << "  时间戳:" << item.timestamp.toString(Qt::ISODate);
            qDebug() << "  时间戳是否有效:" << item.timestamp.isValid();
            qDebug() << "  搜索次数:" << item.searchCount;

            if (!item.keyword.isEmpty() && item.timestamp.isValid()) {
                m_historyItems.append(item);
                qDebug() << "  添加成功";
            } else {
                qDebug() << "  记录无效，跳过";
            }
        }
    }

    ensureHistoryOrder();
    updateHistoryList();

    qDebug() << "加载后历史记录数量:" << m_historyItems.size();
    qDebug() << "=== loadHistory 完成 ===";
}

bool SearchHistoryWidget::isHistoryVisible() const
{
    return m_historyVisible;
}

void SearchHistoryWidget::onHistoryItemClicked(QListWidgetItem* item)
{
    if (item) {
        QString keyword = item->text();
        setKeyword(keyword);
        emit historyItemClicked(keyword);
        emit searchRequested(keyword);
        hideHistory();

        // 将焦点设置回输入框
        setFocusToInput();
    }
}

void SearchHistoryWidget::onSearchPerformed()
{
    QString keyword = this->keyword();
    if (!keyword.trimmed().isEmpty()) {
        addHistory(keyword);
    }
}

void SearchHistoryWidget::onClearHistoryClicked()
{
    clearHistory();
    hideHistory();

    // 显示消息
    showMessage("已清除搜索历史", false, 1500);
}

void SearchHistoryWidget::updateHistoryList()
{
    m_historyList->clear();

    for (const auto& item : m_historyItems) {
        QListWidgetItem* listItem = new QListWidgetItem(item.keyword);

        // 可以添加工具提示显示搜索次数和时间
        QString tooltip = QString("搜索次数: %1\n最后搜索: %2")
                              .arg(item.searchCount)
                              .arg(item.timestamp.toString("yyyy-MM-dd HH:mm"));
        listItem->setToolTip(tooltip);

        m_historyList->addItem(listItem);
    }

    // 如果没有历史记录，隐藏容器
    if (m_historyItems.isEmpty() && m_historyVisible) {
        hideHistory();
    }
}

void SearchHistoryWidget::ensureHistoryOrder()
{
    // 按时间倒序排序
    std::sort(m_historyItems.begin(), m_historyItems.end(),
              [](const HistoryItem& a, const HistoryItem& b) {
                  return a.timestamp > b.timestamp;
              });
}
