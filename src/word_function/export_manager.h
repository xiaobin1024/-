#ifndef EXPORT_MANAGER_H
#define EXPORT_MANAGER_H

#include <QObject>
#include <QList>
#include <xlnt/xlnt.hpp>
#include <QTextDocument>
#include <QPrinter>
#include <QPageSize>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include "word_data.h"


// 前向声明，避免循环包含
class CollectePage;

enum class ExportFormat {
    Excel,
    PDF
};

class ExportManager : public QObject
{
    Q_OBJECT

public:
    static ExportManager* instance();

    // 绑定到收藏页面，连接信号
    void bindToCollectePage(CollectePage* page);

public slots:
    void onExportRequested(ExportFormat format, const QList<WordData>& wordList);

signals:
    void exportStarted(ExportFormat format);
    void exportFinished(bool success, const QString& message);

private:
    explicit ExportManager(QObject* parent = nullptr);
    ~ExportManager();

    bool doExportExcel(const QList<WordData>& list, const QString& filePath);
    bool doExportPdf(const QList<WordData>& list, const QString& filePath);

    QString getDefaultFileName(ExportFormat format) const;
    QString getFilter(ExportFormat format) const;

    static ExportManager* s_instance;

    CollectePage* collecte_page{nullptr};
};

#endif // EXPORT_MANAGER_H
