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
    QString phonetic;       // 音标
    QString definition;     // 释义
    QString partOfSpeech;   // 词性
    QString example;        // 例句

    // 默认构造函数
    WordData() = default;

    // 常用构造函数
    WordData(const QString& w, const QString& p, const QString& d,
             const QString& pos = "", const QString& e = "")
        : word(w)
        , phonetic(p)
        , definition(d)
        , partOfSpeech(pos)
        , example(e)
    {}

    // 检查数据是否有效
    bool isValid() const {
        return !word.isEmpty() && !definition.isEmpty();

    }
    // 获取显示文本（用于调试）
    QString displayText() const {
        if (!isValid()) return "Invalid data";
        return QString("%1 [%2] %3: %4")
            .arg(word)
            .arg(phonetic)
            .arg(partOfSpeech)
            .arg(definition.left(30) + (definition.length() > 30 ? "..." : ""));
    }
};

#endif // WORD_DATA_H
