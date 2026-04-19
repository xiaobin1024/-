#ifndef INTERACTIVE_WORDCARD_H
#define INTERACTIVE_WORDCARD_H

#include "word_card.h"
#include "word_data.h"
#include "word_collect.h"
#include"word_vocabulary.h"
#include<QTextToSpeech>     //文字转语音
#include<QMenu>
/**
 * @brief 交互式单词卡片
 *
 * 在 WordCard 基础上添加：
 * 1. 收藏按钮
 * 2. 发音按钮
 * 3. 添加到生词本按钮
 * 4. 更多操作菜单
 *
 * 这个组件保持简洁，只负责UI和用户交互，
 * 具体业务逻辑通过信号传递给父组件。
 */

class InteractiveWordCard : public WordCard
{
    Q_OBJECT
public:
     explicit InteractiveWordCard(const WordData& data,
                                 WordCollect* wordCollector,
                                 WordVocabulary* wordVocabulary,
                                 QWidget* parent = nullptr);

    // 状态获取
    bool isFavorite() const { return m_isFavorite; }
    bool isAddedToVocabulary() const { return m_isAddedToVocabulary; }

    // 状态设置
    void setFavorite(bool favorite);
    void setAddedToVocabulary(bool added);

    //主题设置
    void cardThemeChanged();

signals:
    // 操作信号
    void favoriteToggled(const QString& word, bool favorite);
    void pronunciationRequested(const QString& word);
    void addToVocabularyRequested(const WordData& data);
    void removeFromVocabularyRequested(const QString& word);
    void editRequested(const WordData& data);
    void shareRequested(const WordData& data);

protected:
    // 重写主题变化处理
    void onThemeChanged() override;
    // 重写样式更新
    void updateCardStyle() override;

private slots:
    void onFavoriteButtonClicked();
    void onPronunciationButtonClicked();
    void onAddToVocabularyButtonClicked();
    void onMoreActionsButtonClicked();
    void onCollectStatusChanged( QString& word,bool isCollected);
    void onVocabularyStatusChanged(const QString& word, bool isVocabulary);


private:
    void initInteractiveUI();
    void setupConnections();
    void updateButtonStyles();
    void createMoreActionsMenu();

    // 按钮控件
    QPushButton* m_favoriteButton{nullptr};
    QPushButton* m_pronunciationButton{nullptr};
    QPushButton* m_addToVocabularyButton{nullptr};
    QPushButton* m_moreActionsButton{nullptr};

    // 状态
    bool m_isFavorite{false};
    bool m_isAddedToVocabulary{false};

    // 布局
    class InteractiveWordCardPrivate;
    InteractiveWordCardPrivate* d;

     WordData m_wordData;
     WordCollect* m_wordCollector{nullptr}; // 用于发送请求
     WordVocabulary* m_wordVocabulary{nullptr};

     //文本转语音对象
     QTextToSpeech *speech;

};

#endif // INTERACTIVE_WORDCARD_H
