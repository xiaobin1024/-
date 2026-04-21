#ifndef VOCABULARY_PAGE_H
#define VOCABULARY_PAGE_H

#include "base\base_widget.h"
#include"word_data.h"
#include"word_collect.h"
#include"word_vocabulary.h"
#include"export_manager.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
class VocabularyPage :public BaseWidget
{
    Q_OBJECT
public:
    explicit VocabularyPage(QWidget *parent = nullptr);
    ~VocabularyPage();

    void initUI() override;
    void setupLayout() override;
    void updateWidgetStyles() override;

// 用于接收并展示生词列表的槽函数
private slots:
    void onVocabularyListSuccess(const QList<WordData>& VocabularyList);
    void onVocabularyListFailed(const QString& errorMessage);
    void onBackButtonClicked();
    void onExportExcelClicked();
    void onExportPdfClicked();

    // 添加生词列表查询函数
    void requestQueryVocabularyList();
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

#endif // VOCABULARY_PAGE_H
