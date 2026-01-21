#ifndef TEST_WORD_CARD_H
#define TEST_WORD_CARD_H

#include "base_widget.h"
#include "word_card.h"
#include <QTimer>
#include<QApplication>

class TestWordCard : public BaseWidget
{
    Q_OBJECT

public:
    explicit TestWordCard(QWidget* parent = nullptr)
        : BaseWidget(parent)
    {
        setupUI();
        setupConnections();
    }

private:
    void setupUI()
    {
        // 设置窗口标题
        setWindowTitle("BaseWidget 集成测试");

        // 设置窗口大小
        resize(400, 500);

        // 使用 BaseWidget 的 createLabel 创建标题
        m_titleLabel = createLabel("WordCard 测试", "title");
        m_mainLayout->addWidget(m_titleLabel);

        // 添加分隔线
        m_mainLayout->addWidget(createLabel("", "caption"));

        // 创建 WordCard
        WordData testData(
            "Hello",
            "/həˈləʊ/",
            "int. 你好；喂\n这是一个简单的问候语，用于开始对话。",
            "int.",
            "Hello, how are you today?"
            );

        m_wordCard = new WordCard(testData, this);
        m_mainLayout->addWidget(m_wordCard);

        // 添加弹性空间
        m_mainLayout->addStretch();

        // 创建控制面板
        createControlPanel();

        // 添加状态标签
        m_statusLabel = createLabel("就绪 - 双击卡片可展开/折叠例句", "caption");
        m_mainLayout->addWidget(m_statusLabel);

        // 初始更新状态
        updateStatus();
    }

    void createControlPanel()
    {
        // 使用 BaseWidget 的 createGroupBox
        auto controlGroup = createGroupBox("控制面板");
        auto controlLayout = new QVBoxLayout(controlGroup);
        controlLayout->setSpacing(8);

        // 使用 BaseWidget 的 createButtonLayout
        auto buttonLayout = createButtonLayout();

        // 使用 BaseWidget 的 createPrimaryButton
        m_themeButton = createPrimaryButton("切换主题");
        m_messageButton = createPrimaryButton("测试消息");
        m_loadingButton = createPrimaryButton("测试加载");

        buttonLayout->addWidget(m_themeButton);
        buttonLayout->addWidget(m_messageButton);
        buttonLayout->addWidget(m_loadingButton);

        controlLayout->addLayout(buttonLayout);

        // 创建输入框测试区域
        auto inputLayout = createButtonLayout();

        // 使用 BaseWidget 的 createLabel
        inputLayout->addWidget(createLabel("新单词:", "normal"));

        // 使用 BaseWidget 的 createLineEdit
        m_wordInput = createLineEdit("输入新单词");

        // 使用 BaseWidget 的 createSecondaryButton
        m_addButton = createSecondaryButton("添加");

        inputLayout->addWidget(m_wordInput, 1);
        inputLayout->addWidget(m_addButton);

        controlLayout->addLayout(inputLayout);

        m_mainLayout->addWidget(controlGroup);
    }

    void setupConnections()
    {
        // 主题切换
        connect(m_themeButton, &QPushButton::clicked, this, &TestWordCard::onThemeToggle);

        // 消息测试
        connect(m_messageButton, &QPushButton::clicked, this, &TestWordCard::onTestMessage);

        // 加载测试
        connect(m_loadingButton, &QPushButton::clicked, this, &TestWordCard::onTestLoading);

        // 添加单词
        connect(m_addButton, &QPushButton::clicked, this, &TestWordCard::onAddWord);

        // WordCard 信号
        if (m_wordCard) {
            connect(m_wordCard, &WordCard::clicked, this, [this]() {
                qDebug() << "WordCard 点击";
                showMessage("卡片被点击", false, 1000);
            });

            connect(m_wordCard, &WordCard::doubleClicked, this, [this]() {
                qDebug() << "WordCard 双击";
                showMessage("卡片被双击", false, 1000);
            });
        }
    }

private slots:
    void onThemeToggle()
    {
        if (uiTheme() == UITheme::Light) {
            setUITheme(UITheme::Dark);
            qDebug() << "切换到暗色主题";
        } else {
            setUITheme(UITheme::Light);
            qDebug() << "切换到亮色主题";
        }
        updateStatus();
    }

    void onTestMessage()
    {
        if (m_wordCard) {
            static int messageCount = 0;
            QString message = QString("测试消息 %1").arg(++messageCount);

            bool isError = (messageCount % 2 == 0);
            m_wordCard->showMessage(message, isError, 2000);

            qDebug() << (isError ? "错误消息:" : "普通消息:") << message;
        }
    }

    void onTestLoading()
    {
        if (m_wordCard) {
            m_wordCard->showLoading("正在加载...");
            qDebug() << "开始加载";

            QTimer::singleShot(2000, this, [this]() {
                if (m_wordCard) {
                    m_wordCard->hideLoading();
                    m_wordCard->showMessage("加载完成", false, 1000);
                    qDebug() << "加载完成";
                }
            });
        }
    }

    void onAddWord()
    {
        QString word = m_wordInput->text().trimmed();
        if (word.isEmpty()) {
            showMessage("请输入单词", true, 1500);
            return;
        }

        // 创建新的单词数据
        WordData newData(
            word,
            "/test/",
            QString("这是新添加的单词: %1").arg(word),
            "n.",
            QString("Example sentence for %1").arg(word)
            );

        if (m_wordCard) {
            m_wordCard->setWordData(newData);
            showMessage(QString("已更新为: %1").arg(word), false, 1500);
            qDebug() << "更新单词:" << word;
        }
    }

    void updateStatus()
    {
        if (m_statusLabel) {
            QString themeText = (uiTheme() == UITheme::Light) ? "亮色主题" : "暗色主题";
            m_statusLabel->setText(QString("状态: 就绪 | 主题: %1").arg(themeText));
        }
    }

protected:
    void onThemeChanged() override
    {
        BaseWidget::onThemeChanged();
        updateStatus();
    }

private:
    // UI 组件
    WordCard* m_wordCard{nullptr};

    // 使用 BaseWidget 创建的组件
    QLabel* m_titleLabel{nullptr};
    QLabel* m_statusLabel{nullptr};
    QPushButton* m_themeButton{nullptr};
    QPushButton* m_messageButton{nullptr};
    QPushButton* m_loadingButton{nullptr};
    QPushButton* m_addButton{nullptr};
    QLineEdit* m_wordInput{nullptr};


};


#endif // TEST_WORD_CARD_H
