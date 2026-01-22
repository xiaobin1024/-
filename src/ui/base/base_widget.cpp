// base_widget.cpp - 第2版（添加样式管理）
#include "base_widget.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QProgressBar>
#include <QDebug>

//样式生成辅助函数
namespace {
    //生成基础的边框圆角样式
    QString baseBorderStyle(const QString& radius="6px"){
        return QString("border-radius: %1;").arg(radius);
    }
    //生成内边距样式
    QString paddingStyle(int vertical,int horizontal){
        return QString("padding: %1px %2px;").arg(vertical).arg(horizontal);
    }
    //生成字体样式
    QString fontStyle(const QString& size="14px",const QString& weigth="normal"){
        return QString("font-size: %1; font-weigth: %2;").arg(size,weigth);
    }
    //生成边框样式
    QString borderStyle(const QString& color,const QString& width="1px"){
        return QString("border: %1 solid %2;").arg(width,color);
    }
}

BaseWidget::BaseWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
{
    // 加载颜色配置
    loadColors();

    // 设置基本样式
    updateWidgetStyles();

    // 配置主布局
    m_mainLayout->setContentsMargins(16, 16, 16, 16);
    m_mainLayout->setSpacing(12);
    setLayout(m_mainLayout);
}

void BaseWidget::showMessage(const QString& message, bool isError,int duration)
{
    qDebug() << "显示消息：" << message << "，持续时间：" << duration << "ms";

    clearMessage();

    QLabel* messageLabel = new QLabel(message, this);
    messageLabel->setProperty("message", true);
    messageLabel->setProperty("error", isError);

    // 使用样式辅助函数
    QString type = isError ? "error" : "success";
    QString style = createMessageStyle(type);
    messageLabel->setStyleSheet(style);
    messageLabel->setWordWrap(true);

    m_mainLayout->insertWidget(0, messageLabel);
    m_messageLabels.append(messageLabel);
    m_currentMessage = message;

    if (duration > 0) {
        setupMessageTimer(duration);
    }
}

QString BaseWidget::createMessageStyle(const QString& type) const
{
    QString bgColor, textColor, borderColor;

    if (type == "error") {
        bgColor = m_colors.value("message-error-bg", "#fee2e2");
        textColor = m_colors.value("message-error-text", "#991b1b");
        borderColor = m_colors.value("message-error-border", "#fca5a5");
    } else {
        bgColor = m_colors.value("message-success-bg", "#dcfce7");
        textColor = m_colors.value("message-success-text", "#166534");
        borderColor = m_colors.value("message-success-border", "#86efac");
    }

    return QString(
               "QLabel {"
               "  color: %1;"
               "  background-color: %2;"
               "  border: 1px solid %3;"
               "  border-radius: 6px;"
               "  padding: 8px 12px;"
               "  font-size: 14px;"
               "  margin: 4px 0;"
               "  min-height: 20px;"
               "}"
               ).arg(textColor, bgColor, borderColor);
}

void BaseWidget::clearMessage()
{
    qDebug() << "开始清除所有消息，当前消息数量：" << m_messageLabels.count();

    // 停止消息定时器
    if (m_messageTimer && m_messageTimer->isActive()) {
        m_messageTimer->stop();
        qDebug() << "已停止消息定时器";
    }

    // 移除并删除所有消息标签
    for (QLabel* label : m_messageLabels) {
        if (label) {
            qDebug() << "删除消息标签：" << label->text();
            m_mainLayout->removeWidget(label);
            label->deleteLater();
        }
    }

    // 清空列表
    m_messageLabels.clear();
    m_currentMessage.clear();

    qDebug() << "消息清除完成";
}

void BaseWidget::showLoading(const QString& message)
{
    // 创建加载指示器
    auto loadingWidget = new QWidget(this);
    loadingWidget->setStyleSheet(createLoadingWidgetStyle());

    auto layout = new QVBoxLayout(loadingWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    // 进度条
    auto progressBar = new QProgressBar(loadingWidget);
    progressBar->setRange(0, 0);
    progressBar->setFixedSize(60, 60);
    progressBar->setStyleSheet(createProgressBarStyle());

    // 加载文字
    auto label = new QLabel(message, loadingWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(createLoadingLabelStyle());

    layout->addWidget(progressBar, 0, Qt::AlignCenter);
    layout->addWidget(label, 0, Qt::AlignCenter);

    // 添加到布局
    m_mainLayout->addWidget(loadingWidget, 0, Qt::AlignCenter);

    // 存储引用以便稍后移除
    loadingWidget->setProperty("loading", true);

    // 更新状态
    setUIState(UIState::Loading);
}
QString BaseWidget::createLoadingWidgetStyle() const
{
    return QString(
        "QWidget {"
        "  background-color: rgba(255, 255, 255, 220);"
        "  border-radius: 8px;"
        "}"
        );
}
QString BaseWidget::createProgressBarStyle() const
{
    return QString(
               "QProgressBar {"
               "  border: none;"
               "  background-color: transparent;"
               "}"
               "QProgressBar::chunk {"
               "  background-color: %1;"
               "  border-radius: 30px;"
               "}"
               ).arg(getColor("primary"));
}
QString BaseWidget::createLoadingLabelStyle() const
{
    return QString(
               "QLabel {"
               "  color: %1;"
               "  font-size: 14px;"
               "}"
               ).arg(getColor("text-primary"));
}

void BaseWidget::hideLoading()
{
    // 查找并移除加载指示器
    for (int i = 0; i < m_mainLayout->count(); ++i) {
        QLayoutItem* item = m_mainLayout->itemAt(i);
        if (item) {
            QWidget* widget = item->widget();
            if (widget && widget->property("loading").isValid()) {  // 使用 isValid 检查属性是否存在
                if (widget->property("loading").toBool()) {
                    m_mainLayout->removeWidget(widget);
                    widget->deleteLater();
                    break;
                }
            }
        }
    }

    // 更新状态
    setUIState(UIState::Normal);
}

void BaseWidget::setUIState(UIState state)
{
    if (m_uiState != state) {
        m_uiState = state;
        onStateChanged();
    }
}

void BaseWidget::setUITheme(UITheme theme)
{
    if (m_uiTheme != theme) {
        m_uiTheme = theme;
        loadColors();  // 重新加载颜色
        updateWidgetStyles();  // 更新样式
        onThemeChanged();
    }
}

QVBoxLayout* BaseWidget::createMainLayout()
{
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    return layout;
}

QHBoxLayout* BaseWidget::createButtonLayout()
{
    auto layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    return layout;
}

QFormLayout* BaseWidget::createFormLayout()
{
    auto layout = new QFormLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(12);
    return layout;
}

QString BaseWidget::createButtonStyle(const QString& type) const
{
    if (type == "primary") {
        return QString(
                   "QPushButton {"
                   "  background-color: %1;"
                   "  color: white;"
                   "  border: none;"
                   "  border-radius: 6px;"
                   "  padding: 10px 20px;"
                   "  font-size: 14px;"
                   "  font-weight: 500;"
                   "}"
                   "QPushButton:hover {"
                   "  background-color: %2;"
                   "}"
                   "QPushButton:pressed {"
                   "  background-color: %2;"
                   "  padding: 11px 19px 9px 21px;"
                   "}"
                   "QPushButton:disabled {"
                   "  background-color: %3;"
                   "  color: %4;"
                   "}"
                   ).arg(getColor("primary"),
                 getColor("primary-dark"),
                 getColor("secondary-light"),
                 getColor("text-disabled"));
    } else { // secondary
        return QString(
                   "QPushButton {"
                   "  background-color: transparent;"
                   "  color: %1;"
                   "  border: 1px solid %1;"
                   "  border-radius: 6px;"
                   "  padding: 9px 19px;"
                   "  font-size: 14px;"
                   "  font-weight: 500;"
                   "}"
                   "QPushButton:hover {"
                   "  background-color: %2;"
                   "}"
                   "QPushButton:pressed {"
                   "  background-color: %2;"
                   "}"
                   "QPushButton:disabled {"
                   "  color: %3;"
                   "  border-color: %3;"
                   "}"
                   ).arg(getColor("primary"),
                 getColor("primary-light") + "20", // 20%透明度
                 getColor("text-disabled"));
    }
}

QPushButton* BaseWidget::createPrimaryButton(const QString& text)
{
    auto button = new QPushButton(text, this);
    button->setStyleSheet(createButtonStyle("primary"));
    return button;
}

QPushButton* BaseWidget::createSecondaryButton(const QString& text)
{
    auto button = new QPushButton(text, this);
    button->setStyleSheet(createButtonStyle("secondary"));
    return button;
}

QString BaseWidget::createLineEditStyle() const
{
    return QString(
               "QLineEdit {"
               "  background-color: %1;"
               "  border: 1px solid %2;"
               "  border-radius: 4px;"
               "  padding: 10px 12px;"
               "  font-size: 14px;"
               "  color: %3;"
               "}"
               "QLineEdit:hover {"
               "  border-color: %4;"
               "}"
               "QLineEdit:focus {"
               "  border-color: %5;"
               "  outline: none;"
               "}"
               "QLineEdit:disabled {"
               "  background-color: %6;"
               "  color: %7;"
               "}"
               ).arg(getColor("surface"),
             getColor("border"),
             getColor("text-primary"),
             getColor("secondary"),
             getColor("primary"),
             getColor("surface"),
             getColor("text-disabled"));
}

QLineEdit* BaseWidget::createLineEdit(const QString& placeholder)
{
    auto lineEdit = new QLineEdit(this);

    lineEdit->setStyleSheet(createLineEditStyle());

    if (!placeholder.isEmpty()) {
        lineEdit->setPlaceholderText(placeholder);
    }

    return lineEdit;
}

void BaseWidget::updateLabelStyle(QLabel* label, const QString& type)
{
    QString colorRole;
    QString fontSize;
    QString fontWeight;

    // 确定颜色角色和字体样式
    if (type == "title") {
        colorRole = "label-title";
        fontSize = "18px";
        fontWeight = "bold";
    } else if (type == "subtitle") {
        colorRole = "label-subtitle";
        fontSize = "16px";
        fontWeight = "600";
    } else if (type == "caption") {
        colorRole = "label-caption";
        fontSize = "12px";
        fontWeight = "normal";
    } else { // normal
        colorRole = "label-normal";
        fontSize = "14px";
        fontWeight = "normal";
    }

    // 获取颜色，如果指定颜色不存在则回退到通用颜色
    QString color = m_colors.contains(colorRole)
                        ? getColor(colorRole)
                        : (type == "caption" ? getColor("text-secondary") : getColor("text-primary"));

    QString style = QString(
                        "QLabel {"
                        "  font-size: %1;"
                        "  font-weight: %2;"
                        "  color: %3;"
                        "}"
                        ).arg(fontSize, fontWeight, color);

    label->setStyleSheet(style);
}

QLabel* BaseWidget::createLabel(const QString& text, const QString& type)
{
    auto label = new QLabel(text, this);

    // 存储类型信息，便于后续更新
    label->setProperty("labelType", type);

    // 添加到标签列表
    m_createdLabels.append(label);

    // 应用样式
    updateLabelStyle(label, type);

    return label;
}

QString BaseWidget::getColor(const QString& role) const
{
    return m_colors.value(role, "#000000");
}

void BaseWidget::updateWidgetStyles()
{
    QString bgColor = getColor("background");
    setStyleSheet(QString("BaseWidget { background-color: %1; }").arg(bgColor));

    // 更新所有QGroupBox
    for (auto groupBox : m_createdGroupBoxes) {
        updateGroupBoxStyle(groupBox);
    }

    // 更新所有QLabel
    for (auto label : m_createdLabels) {
        QString type = label->property("labelType").toString();
        if (type.isEmpty()) {
            type = "normal";
        }
        updateLabelStyle(label, type);
    }
}

void BaseWidget::onThemeChanged()
{
    // 子类可以重写这个函数来响应主题变化
}

void BaseWidget::onStateChanged()
{
    // 子类可以重写这个函数来响应状态变化
}

void BaseWidget::loadColors()
{
    if (m_uiTheme == UITheme::Dark) {
        // 暗色主题
        m_colors = {
            {"primary", "#3b82f6"},
            {"primary-dark", "#2563eb"},
            {"primary-light", "#60a5fa"},
            {"secondary", "#94a3b8"},
            {"secondary-dark", "#64748b"},
            {"secondary-light", "#cbd5e1"},
            {"success", "#10b981"},
            {"warning", "#f59e0b"},
            {"error", "#ef4444"},
            {"background", "#1e293b"},
            {"surface", "#334155"},
            {"groupbox-surface", "#2d3748"},    // 专门用于GroupBox
            {"groupbox-border", "#4a5568"},     // 专门用于GroupBox
            {"text-primary", "#f1f5f9"},
            {"text-secondary", "#cbd5e1"},
            {"text-disabled", "#94a3b8"},
            {"border", "#475569"},
            // 添加标签专用颜色 - 提高标题对比度
            {"label-title", "#ffffff"},          // 标题使用纯白色
            {"label-subtitle", "#f1f5f9"},       // 副标题使用text-primary
            {"label-caption", "#94a3b8"},        // 说明文字使用text-disabled颜色
            {"label-normal", "#f1f5f9"},          // 普通标签使用text-primary
            // 消息颜色
            {"message-success-bg", "#166534"},
            {"message-success-text", "#dcfce7"},
            {"message-success-border", "#22c55e"},
            {"message-error-bg", "#991b1b"},
            {"message-error-text", "#fee2e2"},
            {"message-error-border", "#ef4444"}
        };
    } else {
        // 亮色主题（默认）
        m_colors = {
            {"primary", "#2563eb"},
            {"primary-dark", "#1d4ed8"},
            {"primary-light", "#60a5fa"},
            {"secondary", "#64748b"},
            {"secondary-dark", "#475569"},
            {"secondary-light", "#94a3b8"},
            {"success", "#10b981"},
            {"warning", "#f59e0b"},
            {"error", "#ef4444"},
            {"background", "#ffffff"},
            {"surface", "#f8fafc"},
            {"groupbox-surface", "#f8fafc"},    // 亮色主题与surface相同
            {"groupbox-border", "#e2e8f0"},     // 亮色主题与border相同
            {"text-primary", "#1e293b"},
            {"text-secondary", "#64748b"},
            {"text-disabled", "#94a3b8"},
            {"border", "#e2e8f0"},
            // 添加标签专用颜色
            {"label-title", "#1e293b"},          // 标题使用text-primary
            {"label-subtitle", "#1e293b"},       // 副标题使用text-primary
            {"label-caption", "#64748b"},        // 说明文字使用text-secondary
            {"label-normal", "#1e293b"},          // 普通标签使用text-primary
            // 消息颜色
            {"message-success-bg", "#dcfce7"},
            {"message-success-text", "#166534"},
            {"message-success-border", "#86efac"},
            {"message-error-bg", "#fee2e2"},
            {"message-error-text", "#991b1b"},
            {"message-error-border", "#fca5a5"}
        };
    }
}

QGroupBox* BaseWidget::createGroupBox(const QString& title)
{
    QGroupBox* groupBox = new QGroupBox(title, this);
    m_createdGroupBoxes.append(groupBox);
    updateGroupBoxStyle(groupBox);
    return groupBox;
}
void BaseWidget::updateGroupBoxStyle(QGroupBox* groupBox)
{
    QString borderColor = m_colors.contains("groupbox-border")
    ? getColor("groupbox-border") : getColor("border");
    QString textColor = getColor("text-primary");
    QString surfaceColor = m_colors.contains("groupbox-surface")
                               ? getColor("groupbox-surface") : getColor("surface");

    QString styleSheet = QString(
                             "QGroupBox {"
                             "  border: 2px solid %1;"
                             "  border-radius: 8px;"
                             "  margin-top: 12px;"
                             "  padding-top: 12px;"
                             "  font-weight: bold;"
                             "  color: %2;"
                             "  background-color: %3;"
                             "}"
                             "QGroupBox::title {"
                             "  subcontrol-origin: margin;"
                             "  subcontrol-position: top left;"
                             "  left: 12px;"
                             "  padding: 0 8px 0 8px;"
                             "  background-color: %3;"
                             "}"
                             ).arg(borderColor, textColor, surfaceColor);

    groupBox->setStyleSheet(styleSheet);
}

void BaseWidget::setupMessageTimer(int duration)
{
    if (!m_messageTimer) {
        m_messageTimer = new QTimer(this);
        m_messageTimer->setSingleShot(true);
        connect(m_messageTimer, &QTimer::timeout, this, &BaseWidget::clearMessage);
    }

    m_messageTimer->stop();
    m_messageTimer->start(duration);
    qDebug() << "已设置消息自动清除定时器：" << duration << "ms";
}
