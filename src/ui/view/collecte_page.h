#ifndef COLLECTE_PAGE_H
#define COLLECTE_PAGE_H

#include "base\base_widget.h"
#include"word_data.h"
#include"word_collect.h"
#include"word_vocabulary.h"
#include"export_manager.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>

class CollectePage : public BaseWidget
{
    Q_OBJECT

public:
    explicit CollectePage(QWidget *parent = nullptr);
    ~CollectePage();

    void initUI() override;
    void setupLayout() override;
     void updateWidgetStyles() override;

    // 用于接收并展示收藏列表的槽函数
private slots:
    void onCollectListSuccess(const QList<WordData>& wordList);
    void onCollectListFailed(const QString& errorMessage);
     void onBackButtonClicked();
    void onExportExcelClicked();
    void onExportPdfClicked();

    // 添加收藏列表查询函数
    void requestQueryCollectList();

private:

signals:
    void showMainPageRequested();
    void exportRequested(ExportFormat format, const QList<WordData>& wordList);
private:
    QList<WordData> m_currentWordList;  // 当前显示的单词列表
    QScrollArea* m_scrollArea{nullptr};
    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    QLabel* m_placeholderLabel{nullptr}; // 提示标签
    QPushButton *m_refreshButton{nullptr};  // 新增刷新按钮
    QPushButton *m_backButton{nullptr};
    QPushButton* m_exportExcelButton{nullptr};
    QPushButton* m_exportPdfButton{nullptr};

    WordCollect* m_wordCollect{nullptr};
    WordVocabulary* m_wordVocabulary{nullptr};
    ExportManager* m_exportManager{nullptr};

};

#endif // COLLECTE_PAGE_H
