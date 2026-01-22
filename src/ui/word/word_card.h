#ifndef WORD_CARD_H
#define WORD_CARD_H

/**
 * @brief 单词卡片基类
 *
 * 这是一个基础的单词展示组件，继承自 BaseWidget，用于显示单词的基本信息。
 * 特性：
 * 1. 只负责展示，不包含复杂的交互功能
 * 2. 完全使用 BaseWidget 的样式系统和辅助函数
 * 3. 支持双击展开/折叠例句
 * 4. 响应主题切换
 * 5. 尺寸自适应内容
 *
 * 复杂的功能（如收藏、发音、添加到生词本等）应由继承此类的子组件实现。
 */

#include "/base/base_widget.h"
#include "word_data.h"
#include <QWidget>

class WordCard :public BaseWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父组件指针
     */
    explicit WordCard(QWidget* parent = nullptr);

    /**
     * @brief 构造函数
     * @param data 单词数据
     * @param parent 父组件指针
     */
    explicit WordCard(const WordData& data, QWidget* parent = nullptr);

    /**
     * @brief 设置单词数据
     * @param data 单词数据
     */
    void setWordData(const WordData& data);

    /**
     * @brief 获取单词数据
     * @return 当前的单词数据
     */
    const WordData& getWordData() const { return m_data; }

    /**
     * @brief 设置展开状态
     * @param expanded true表示展开（显示例句），false表示折叠
     */
    void setExpanded(bool expanded);

    /**
     * @brief 获取展开状态
     * @return 当前是否展开
     */
    bool isExpanded() const { return m_expanded; }

    /**
     * @brief 获取建议尺寸
     * @return 根据内容计算出的建议尺寸
     */
    QSize sizeHint() const override;

    /**
     * @brief 获取最小建议尺寸
     * @return 最小尺寸
     */
    QSize minimumSizeHint() const override;

signals:
    /**
     * @brief 点击信号
     * 当用户点击卡片时发出
     */
    void clicked();

    /**
     * @brief 双击信号
     * 当用户双击卡片时发出
     */
    void doubleClicked();
protected:
    /**
     * @brief 主题变化事件
     * 重写 BaseWidget 的虚函数，当主题变化时更新卡片样式
     */
    void onThemeChanged() override;

    /**
     * @brief 鼠标按下事件处理
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief 鼠标双击事件处理
     * @param event 鼠标事件
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    /**
     * @brief 绘制事件处理
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent* event) override;
private:
    // 初始化UI
    void initUI();

    // 更新内容显示
    void updateContent();

    // 更新卡片样式
    void updateCardStyle();

    // 更新词性标签样式
    void updatePartOfSpeechStyle();
private:
    WordData m_data;            // 单词数据
    bool m_expanded{false};     // 是否展开显示例句

    // UI组件 - 全部使用 BaseWidget 的辅助函数创建
    QLabel* m_wordLabel{nullptr};          // 单词标签
    QLabel* m_phoneticLabel{nullptr};      // 音标标签
    QLabel* m_partOfSpeechLabel{nullptr};  // 词性标签
    QLabel* m_definitionLabel{nullptr};    // 释义标签
    QLabel* m_exampleLabel{nullptr};       // 例句标签
};


#endif // WORD_CARD_H
