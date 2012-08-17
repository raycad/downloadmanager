/*!
 * \file downloadthread.h
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QThread>
#include <QNetworkReply>
#include <QTimer>
#include <QFile>
#include "downloadmanager.h"

class Download;
class DownloadThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloadThread(Download *download);
    ~DownloadThread();

    /*!
     * \brief stop the thread
     */
    void stop();

signals:
    /*!
     * \brief emitted when received changed
     * \param downloadId: id of the download
     * \param remainTime: remain time of the download
     */
    void downloadTimeRemain(int downloadId, int remainTime);

    /*!
     * \brief emitted when not receive data
     * \param downloadId: id of download
     */
    void noReceivedData(int downloadId);

    /*!
     * \brief emitted when a download is finished
     * \param downloadId: id of download
     * \param filePath: full path of download file saved
     * \param urlType: type of download url
     */
    void downloadFinished();

    /*!
     * \brief emitted when an error occurs
     * \param downloadId: id of download
     * \param errorCode: look up the results from DownloadErrorCode
     */
    void downloadError(int downloadId, DownloadManager::DownloadErrorCode error);

    /*!
     * \brief emitted when an ssl error occurs
     * \param downloadId: id of download
     * \param errors: QSslError
     */
    void downloadSslErrors(const QList<QSslError> &errors);

public:
    /*!
     * \brief get current remain time
     * \returns current remain time
     * \returns -1 if it does not receive data
     */
    int getCurrentRemainTime();

protected slots:
    // Slot when download finished
    void slotDownloadFinished();

    // Slot when getting progress changes
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // Slot when getting errors
    void slotDownloadError(QNetworkReply::NetworkError error);

    // Update download progress
    void updateDownloadProgress();

    // Update download timeout
    void updateDownloadTimeout();

    // Write buffer to file
    void slotReadyRead();

protected:

    void run();

    void init();

    void disconnectSignals();

private:
    Download *m_download;

    QTimer *m_downloadProgressTimer;
    QTimer *m_downloadTimeoutTimer;

    QTime *m_downloadTime;

    QFile m_output; // File to store dta stream

    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_networkReply;

    bool m_canUpdateProgress;
    bool m_checkDownloadTimeout;
    bool m_hasData;

    int m_currentRemainTime; // current remain time
};

#endif // DOWNLOADTHREAD_H
