#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloadmanager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    bool initialize();

protected slots:

    void onDownloadTimeRemain(int downloadId, int remainTime);

    void onNoReceivedData(int downloadId);

    void onDownloadFinished(int contactId, int downloadId, const QString &filePath, int urlType);

    void onDownloadError(int downloadId, int errorCode);

    void onContactDownloadError(int contactId, int errorCode);

    void onDownloadStatusChanged(int contactId, int downloadId, int downloadStatus);

private slots:
    void on_addUrlPushButton_clicked();

    void on_stopDownloadPushButton_clicked();

private:
    Ui::MainWindow *ui;

    DownloadManager m_downloadManager;
};

#endif // MAINWINDOW_H
