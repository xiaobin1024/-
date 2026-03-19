#ifndef WORD_DATA_H
#define WORD_DATA_H
/*
 * 单词数据结构
 *
 * 用于存储单词的基本信息
 */

#include<QString>

struct WordData{
    QString word;           // 单词
    QString meaning;     // 释义
    QString phonetic;       // 音标
    QString example;        // 例句
    QString translation;    // 句意

    // 默认构造函数
    WordData() = default;

    // 常用构造函数
    WordData(const QString& w, const QString& p, const QString& d,
              const QString& e = "",const QString& t="")
        : word(w)
        , phonetic(p)
        , meaning(d)
        , example(e)
        ,translation(t)
    {}

    // 检查数据是否有效
    bool isValid() const {
        return !word.isEmpty() && !meaning.isEmpty();

    }
    // 获取显示文本（用于调试）
    QString displayText() const {
        if (!isValid()) return "Invalid data";
        return QString("%1 [%2] %3: %4")
            .arg(word)
            .arg(meaning.left(30) + (meaning.length() > 30 ? "..." : ""))
            .arg(phonetic)
            .arg(example)
            .arg(translation);
    }
};

#endif // WORD_DATA_H
