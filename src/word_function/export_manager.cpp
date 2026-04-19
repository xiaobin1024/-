#include "export_manager.h"
#include "view/collecte_page.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

ExportManager* ExportManager::s_instance = nullptr;

ExportManager* ExportManager::instance()
{
    if (!s_instance) {
        s_instance = new ExportManager();
    }
    return s_instance;
}

ExportManager::ExportManager(QObject* parent)
    : QObject(parent)
{
}

ExportManager::~ExportManager()
{
}

void ExportManager::bindToCollectePage(CollectePage* page)
{
    // 连接收藏页面的导出信号到管理器的槽
    connect(page, &CollectePage::exportRequested,
            this, &ExportManager::onExportRequested);
}

void ExportManager::onExportRequested(ExportFormat format, const QList<WordData>& wordList)
{
    // 选择保存路径
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                          + "/" + getDefaultFileName(format);

    QString filter = getFilter(format);
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        format == ExportFormat::Excel ? "导出Excel" : "导出PDF",
        defaultPath,
        filter
        );

    if (filePath.isEmpty()) {
        return;  // 用户取消
    }

    emit exportStarted(format);

    bool success = false;
    if (format == ExportFormat::Excel) {
        success = doExportExcel(wordList, filePath);
    } else {
        success = doExportPdf(wordList, filePath);
    }

    emit exportFinished(success, success ? "导出成功" : "导出失败");
}

bool ExportManager::doExportExcel(const QList<WordData>& list, const QString& filePath)
{
    qDebug() << "导出Excel:" << filePath << "数量:" << list.size();
    // TODO: 集成 xlnt 库实现
    return true;
}

bool ExportManager::doExportPdf(const QList<WordData>& list, const QString& filePath)
{
    qDebug() << "导出PDF:" << filePath << "数量:" << list.size();
    // TODO: 集成 PDF 库实现
    return true;
}

QString ExportManager::getDefaultFileName(ExportFormat format) const
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return format == ExportFormat::Excel
               ? "收藏单词_" + timestamp + ".xlsx"
               : "收藏单词_" + timestamp + ".pdf";
}

QString ExportManager::getFilter(ExportFormat format) const
{
    return format == ExportFormat::Excel
               ? "Excel文件 (*.xlsx)"
               : "PDF文件 (*.pdf)";
}
