#include "test_window.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

TestWindow::TestWindow(QWidget* parent)
    :Basewidget(parent)
{
    setupUI();
    setupConnections();
}
void TestWindow::setupUI()
{
    //创建控件
    m_testLabel=new QLabel("这是一个测试窗口",this);
    m_messageInput=new QLineEdit(this);
    m_messageInput->setPlaceholderText("请输入消息内容...");

    m_showButton=new QPushButton("显示消息",this);
    m_clearButton=new QPushButton("清除消息",this);

    //创建布局
    auto inputLayout=new QHBoxLayout();
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_showButton);
    inputLayout->addWidget(m_clearButton);

    //添加到主布局
    m_mainLayout->addWidget(m_testLabel);
    m_mainLayout->addLayout(inputLayout);
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
        message = "这是一条测试消息！";
    }
    showMessage(message);
}

void TestWindow::onClearMessage()
{
    clearMessage();
}
