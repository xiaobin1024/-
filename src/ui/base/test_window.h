#ifndef TEST_WINDOW_H
#define TEST_WINDOW_H

#include"base_widget.h"
#include<QPushButton>
#include<QLineEdit>
#include<QLabel>

class TestWindow: public BaseWidget
{
    Q_OBJECT
public:
    explicit TestWindow(QWidget* parent=nullptr);
public slots:
    void onShowMessage();
    void onClearMessage();
private:
    void setupUI();
    void setupConnections();

    QPushButton* m_showButton{nullptr};
    QPushButton* m_clearButton{nullptr};
    QLineEdit* m_messageInput{nullptr};
    QLabel* m_testLabel{nullptr};
};

#endif // TEST_WINDOW_H
