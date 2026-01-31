#include "word_card.h"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

WordCard::WordCard(QWidget* parent)
    : BaseWidget(parent)  // 调用 BaseWidget 构造函数
{
    initUI();
}

WordCard::WordCard(const WordData& data, QWidget* parent)
    : BaseWidget(parent)  // 调用 BaseWidget 构造函数
    , m_data(data)        // 初始化单词数据
{
    initUI();
    updateContent();  // 初始化后立即更新内容
}

void WordCard::initUI()
{
    // 设置卡片基本尺寸约束
    setMinimumWidth(200);   // 最小宽度
    setMinimumHeight(60);   // 最小高度
    setMaximumWidth(450);   // 最大宽度，避免在宽屏幕上显示过宽

    // 重新配置主布局的边距和间距
    // BaseWidget 构造函数中已设置默认值，这里根据卡片需求调整
    m_mainLayout->setContentsMargins(12, 12, 12, 12);  // 内边距
    m_mainLayout->setSpacing(8);                       // 组件间距

    // 使用 BaseWidget 的 createLabel 函数创建所有标签
    // 这些函数会自动应用样式并管理标签的生命周期

    // 单词标签 - 使用 title 样式
    m_wordLabel = createLabel("", "title");
    m_wordLabel->setWordWrap(true);  // 允许单词换行

    // 音标标签 - 使用 caption 样式
    m_phoneticLabel = createLabel("", "caption");

    // 词性标签 - 需要特殊样式，不使用 createLabel
    m_partOfSpeechLabel = new QLabel(this);
    m_partOfSpeechLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 释义标签 - 使用 normal 样式
    m_definitionLabel = createLabel("", "normal");
    m_definitionLabel->setWordWrap(true);  // 允许释义换行

    // 例句标签 - 使用 caption 样式
    m_exampleLabel = createLabel("", "caption");
    m_exampleLabel->setWordWrap(true);     // 允许例句换行
    m_exampleLabel->hide();                 // 默认隐藏，双击时显示

    //创建一个水平布局来放置英标和词性标签
    QHBoxLayout* middleLayout=new QHBoxLayout();
    middleLayout->setContentsMargins(0,0,0,0);  //取消内边距
    middleLayout->setSpacing(8);  // 音标和词性之间的间距

    // 将音标和词性添加到水平布局
    middleLayout->addWidget(m_phoneticLabel);
    middleLayout->addWidget(m_partOfSpeechLabel);
    middleLayout->addStretch();  // 添加弹性空间，使内容左对齐

    // 将所有组件添加到主布局
    m_mainLayout->addWidget(m_wordLabel);
    m_mainLayout->addLayout(middleLayout);
    m_mainLayout->addWidget(m_definitionLabel);
    m_mainLayout->addWidget(m_exampleLabel);

    // 设置初始样式
    updateCardStyle();
    updatePartOfSpeechStyle();
}

void WordCard::setWordData(const WordData& data)
{
    m_data = data;
    updateContent();  // 数据变化时更新显示
}

void WordCard::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;

        // 控制例句的显示/隐藏
        if (m_expanded && !m_data.example.isEmpty()) {
            m_exampleLabel->show();
        }
        else {
            m_exampleLabel->hide();
        }

        // 通知布局系统尺寸可能已变化
        updateGeometry();
    }
}

QSize WordCard::sizeHint() const
{
    // 根据内容自适应计算建议尺寸

    int width = 320;  // 默认宽度
    int height = 100; // 基础高度（包含内边距和基本间距）

    if (m_wordLabel) {
        // 计算单词标签所需高度
        QFontMetrics fm(m_wordLabel->font());
        int wordHeight = fm.boundingRect(0, 0, width - 24, 0,
                                         Qt::TextWordWrap,
                                         m_wordLabel->text()).height();
        height += wordHeight;
    }

    if (m_definitionLabel) {
        // 计算释义标签所需高度
        QFontMetrics fm(m_definitionLabel->font());
        int textWidth = width - 24;  // 减去内边距
        int definitionHeight = fm.boundingRect(0, 0, textWidth, 0,
                                               Qt::TextWordWrap,
                                               m_definitionLabel->text()).height();
        height += definitionHeight;
    }

    if (m_expanded && m_exampleLabel && m_exampleLabel->isVisible()) {
        // 计算例句标签所需高度
        QFontMetrics fm(m_exampleLabel->font());
        int textWidth = width - 24;
        int exampleHeight = fm.boundingRect(0, 0, textWidth, 0,
                                            Qt::TextWordWrap,
                                            m_exampleLabel->text()).height();
        height += exampleHeight + 8;  // 加上例句与释义的间距
    }

    return QSize(width, height);
}

QSize WordCard::minimumSizeHint() const
{
    // 返回最小建议尺寸
    return QSize(200, 60);
}

void WordCard::onThemeChanged()
{
    // 调用基类的主题更新处理
    BaseWidget::onThemeChanged();

    // 更新卡片特定样式
    updateCardStyle();
    updatePartOfSpeechStyle();
}

void WordCard::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();  // 发出点击信号
    }

    // 调用 QWidget 的事件处理
    QWidget::mousePressEvent(event);
}

void WordCard::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();  // 发出双击信号

        // 双击切换展开状态
        setExpanded(!m_expanded);
    }

    // 调用 QWidget 的事件处理
    QWidget::mouseDoubleClickEvent(event);
}

void WordCard::paintEvent(QPaintEvent* event)
{
    // 支持 QSS 样式表
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // 手动绘制圆角和边框，确保视觉效果一致
    p.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect().adjusted(1, 1, -1, -1);

    // 使用 BaseWidget 的颜色系统获取背景色和边框色
    p.setBrush(QBrush(QColor(getColor("surface"))));
    p.setPen(QPen(QColor(getColor("border")), 1));
    p.drawRoundedRect(rect, 6, 6);

    // 调用 QWidget 的绘制事件处理
    QWidget::paintEvent(event);
}

void WordCard::updateContent()
{
    if (!m_data.isValid()) {
        // 数据无效时显示占位符
        m_wordLabel->setText("No data");
        m_phoneticLabel->setText("");
        m_partOfSpeechLabel->setText("");
        m_definitionLabel->setText("");
        m_exampleLabel->setText("");
        m_exampleLabel->hide();
        return;
    }

    // 更新单词
    m_wordLabel->setText(m_data.word);
    m_wordLabel->setToolTip(m_data.word);

    // 更新音标
    if (!m_data.phonetic.isEmpty()) {
        m_phoneticLabel->setText("/" + m_data.phonetic + "/");
        m_phoneticLabel->setToolTip("音标");
        m_phoneticLabel->show();
    }
    else {
        m_phoneticLabel->hide();
    }

    // 更新词性
    if (!m_data.partOfSpeech.isEmpty()) {
        m_partOfSpeechLabel->setText(m_data.partOfSpeech);
        m_partOfSpeechLabel->setToolTip("词性");
        m_partOfSpeechLabel->show();
    }
    else {
        m_partOfSpeechLabel->hide();
    }

    // 更新释义
    QString definition = m_data.definition;
    definition.replace("\\n", "\n");  // 处理转义换行符

    // 限制最大显示行数，避免卡片过长
    QStringList lines = definition.split("\n");
    if (lines.size() > 3) {
        definition = lines.mid(0, 3).join("\n") + "\n...";
    }

    m_definitionLabel->setText(definition);
    m_definitionLabel->setToolTip(m_data.definition);

    // 更新例句
    if (!m_data.example.isEmpty()) {
        QString example = "\"" + m_data.example + "\"";
        m_exampleLabel->setText(example);
        m_exampleLabel->setToolTip("例句");

        // 根据展开状态决定是否显示
        if (m_expanded) {
            m_exampleLabel->show();
        }
    }
    else {
        m_exampleLabel->hide();
    }

    // 更新词性标签样式
    updatePartOfSpeechStyle();

    // 通知布局系统尺寸可能已变化
    adjustSize();
    updateGeometry();
}

void WordCard::updateCardStyle()
{
    // 使用 BaseWidget 的 getColor 函数获取主题颜色
    // 这里只使用 BaseWidget 中存在的颜色键

    QString backgroundColor = getColor("surface");
    QString borderColor = getColor("border");
    QString hoverBackgroundColor = getColor("primary-light") + "20";  // 20% 透明度
    QString hoverBorderColor = getColor("primary");

    qDebug() << "updateCardStyle - 主题:" << (uiTheme() == UITheme::Light ? "亮色" : "暗色");
    qDebug() << "surface颜色:" << backgroundColor;
    qDebug() << "border颜色:" << borderColor;

    // 设置卡片的样式表
    setStyleSheet(QString(
                      "WordCard {"
                      "  background-color: %1;"
                      "  border: 1px solid %2;"
                      "  border-radius: 6px;"
                      "}"
                      "WordCard:hover {"
                      "  background-color: %3;"
                      "  border-color: %4;"
                      "}"
                      "WordCard QLabel {"
                      "  background-color: transparent;"
                      "}"
                      ).arg(backgroundColor,
                           borderColor,
                           hoverBackgroundColor,
                           hoverBorderColor));
}

void WordCard::updatePartOfSpeechStyle()
{
    // 词性标签需要特殊样式，不使用 BaseWidget 的标签样式
    // 使用主色调作为背景，白色文字

    QString style = QString(
                        "background-color: %1;"
                        "color: white;"
                        "border-radius: 3px;"
                        "padding: 1px 6px;"
                        "font-size: 11px;"
                        "font-weight: 500;"
                        "margin: 0;"
                        ).arg(getColor("primary"));  // 使用主题的主色调

    m_partOfSpeechLabel->setStyleSheet(style);
}


