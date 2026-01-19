// test_window.cpp - 第2版（添加样式测试）
#include "test_window.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

TestWindow::TestWindow(QWidget* parent)
    : BaseWidget(parent)
{
    setupUI();
    setupConnections();
}

void TestWindow::setupUI()
{
    // 创建标题
    m_testLabel = createLabel("BaseWidget 功能测试", "title");

    // 创建输入区域 - 使用 BaseWidget 提供的函数
    auto inputGroup = createGroupBox("消息测试");

    auto inputLayout = new QVBoxLayout(inputGroup);
    m_messageInput = createLineEdit("请输入要显示的消息...");

    auto buttonLayout = createButtonLayout();
    m_showButton = createPrimaryButton("显示消息");
    auto showErrorButton = createPrimaryButton("显示错误");
    m_clearButton = createPrimaryButton("清除消息");

    buttonLayout->addWidget(m_showButton);
    buttonLayout->addWidget(showErrorButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();

    inputLayout->addWidget(m_messageInput);
    inputLayout->addLayout(buttonLayout);
    // 创建功能区域
    auto functionGroup = createGroupBox("功能测试");
    //functionGroup->setStyleSheet(inputGroup->styleSheet());

    auto functionLayout = new QVBoxLayout(functionGroup);

    auto loadingLayout = createButtonLayout();
    auto loadingButton = createSecondaryButton("显示加载指示器");
    auto hideLoadingButton = createSecondaryButton("隐藏加载指示器");

    loadingLayout->addWidget(loadingButton);
    loadingLayout->addWidget(hideLoadingButton);
    loadingLayout->addStretch();

    auto themeLayout = createButtonLayout();
    auto themeButton = createSecondaryButton("切换主题");

    themeLayout->addWidget(themeButton);
    themeLayout->addStretch();

    functionLayout->addLayout(loadingLayout);
    functionLayout->addWidget(createLabel("点击'切换主题'在亮色/暗色主题间切换", "caption"));
    functionLayout->addLayout(themeLayout);

    // 添加到主布局
    m_mainLayout->addWidget(m_testLabel);
    m_mainLayout->addWidget(inputGroup);
    m_mainLayout->addWidget(functionGroup);
    m_mainLayout->addStretch();

    // 连接新按钮
    connect(showErrorButton, &QPushButton::clicked, this, [this]() {
        QString message = m_messageInput->text();
        if (message.isEmpty()) {
            message = "这是一条错误消息！";
        }
        showMessage(message, true); // 显示错误消息
    });

    connect(loadingButton, &QPushButton::clicked, this, [this]() {
        showLoading("正在处理您的请求...");
    });

    connect(hideLoadingButton, &QPushButton::clicked, this, [this]() {
        hideLoading();
    });

    connect(themeButton, &QPushButton::clicked, this, [this]() {
        if (uiTheme() == UITheme::Light) {
            setUITheme(UITheme::Dark);
            showMessage("已切换到暗色主题");
        } else {
            setUITheme(UITheme::Light);
            showMessage("已切换到亮色主题");
        }
    });
}

void TestWindow::setupConnections()
{
    connect(m_showButton, &QPushButton::clicked, this, &TestWindow::onShowMessage);
    connect(m_clearButton, &QPushButton::clicked, this, &TestWindow::onClearMessage);
}

void TestWindow::onShowMessage()
{
    QString message = m_messageInput->text();
    if (message.isEmpty()) {
        message = "这是一条普通消息！";
    }
    showMessage(message, false); // 显示普通消息
}

void TestWindow::onClearMessage()
{
    clearMessage();
}
