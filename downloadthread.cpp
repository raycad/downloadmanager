/*!
 * \file downloadthread.cpp
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "downloadthread.h"
#include "download.h"
#include <QDebug>
#include <QTime>

DownloadThread::DownloadThread(Download *download) : QThread(download)
{
    m_download = download;
    m_networkAccessManager = 0;
    m_downloadProgressTimer = 0;
    m_downloadTimeoutTimer = 0;
    m_downloadTime = 0;

    Q_ASSERT(m_download != 0);
}

void DownloadThread::init()
{
    QUrl url = QUrl(m_download->getUrl());
    QNetworkRequest request(url);

    qDebug() << "URL= " << url;

    if (m_networkAccessManager)
        delete m_networkAccessManager;
    m_networkAccessManager = new QNetworkAccessManager();
    Q_ASSERT(m_networkAccessManager != 0);

    if (m_downloadProgressTimer)
        delete m_downloadProgressTimer;
    m_downloadProgressTimer = new QTimer;
    Q_ASSERT(m_downloadProgressTimer != 0);
    connect(m_downloadProgressTimer, SIGNAL(timeout()), this, SLOT(updateDownloadProgress()), Qt::DirectConnection);
    m_downloadProgressTimer->start(1000*DOWNLOAD_PROGRESS_INTERVAL);

    if (m_downloadTimeoutTimer)
        delete m_downloadTimeoutTimer;
    m_downloadTimeoutTimer = new QTimer;
    Q_ASSERT(m_downloadTimeoutTimer != 0);
    connect(m_downloadTimeoutTimer, SIGNAL(timeout()), this, SLOT(updateDownloadTimeout()), Qt::DirectConnection);
    m_downloadTimeoutTimer->start(1000*DOWNLOAD_TIMEOUT);

    if (m_downloadTime)
        delete m_downloadTime;
    m_downloadTime = new QTime;
    Q_ASSERT(m_downloadTime != 0);
    m_downloadTime->start();

    m_canUpdateProgress = true;
    m_hasData = true;
    m_currentRemainTime = -1;

    m_networkReply = m_networkAccessManager->get(request);
    connect(m_networkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)), Qt::DirectConnection);
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(slotDownloadFinished()), Qt::DirectConnection);
    connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotDownloadError(QNetworkReply::NetworkError)), Qt::DirectConnection);
    connect(m_networkReply, SIGNAL(sslErrors(QList<QSslError>)), this, SIGNAL(downloadSslErrors(QList<QSslError>)), Qt::DirectConnection);
    connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()), Qt::DirectConnection);

    QString savedFilePath = m_download->getSavedFilePathName();
    m_output.setFileName(savedFilePath);
    if (!m_output.open(QIODevice::WriteOnly)) {
        emit downloadError(m_download->getId(), DownloadManager::CanNotWriteToDisk);
        exit();
        return; // skip this download
    }
}

void DownloadThread::disconnectSignals()
{
    if (m_downloadProgressTimer) {
        disconnect(m_downloadProgressTimer, SIGNAL(timeout()), this, SLOT(updateDownloadProgress()));
    }

    if (m_downloadTimeoutTimer) {
        disconnect(m_downloadTimeoutTimer, SIGNAL(timeout()), this, SLOT(updateDownloadTimeout()));
    }
}

void DownloadThread::run()
{
    init();

    exec();
}

void DownloadThread::updateDownloadProgress()
{
    m_canUpdateProgress = true;

    if (m_hasData) {
        m_hasData = false;
    } else {
        // emit no received data
        int downloadId = m_download->getId();
        m_currentRemainTime = -1;
        emit noReceivedData(downloadId);
        qDebug() << __PRETTY_FUNCTION__ << " Emitted noReceivedData signal. Download Id = " << downloadId;
    }
}

void DownloadThread::updateDownloadTimeout()
{
    if (m_hasData)
        return;

    // emit timeout signal
    emit downloadError(m_download->getId(), DownloadManager::TimeoutError);
}

void DownloadThread::slotDownloadError(QNetworkReply::NetworkError error)
{
    emit downloadError(m_download->getId(), (DownloadManager::DownloadErrorCode)error);
}

void DownloadThread::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_hasData = true;

    if (m_canUpdateProgress) {
        int elapseTime = 1;
        if (m_downloadTime != 0)
            elapseTime = m_downloadTime->elapsed();

        if (elapseTime == 0)
            elapseTime = 0.001; // When fast download, the elapse time might be equal to 0, reset it to 1 milisecond

        int currentSpeed = 0;
        currentSpeed = bytesReceived*1000/elapseTime; //byte/s

        qint64 remainByte = bytesTotal - bytesReceived;
        if (currentSpeed > 0) {
            int remainTime = remainByte*1000/currentSpeed;
            m_currentRemainTime = remainTime;
            emit downloadTimeRemain(m_download->getId(), remainTime);
            qDebug() << __PRETTY_FUNCTION__ << " Emitted downloadTimeRemain signal" << ", downloadId = " << m_download->getId();
        }

        m_canUpdateProgress = false;
    }
}

void DownloadThread::slotDownloadFinished()
{
    m_currentRemainTime = 0;
    m_output.close();

    if (m_networkReply->error()) {
        qDebug() << __PRETTY_FUNCTION__ << " Download failed" << ", downloadId = " << m_download->getId() << ": " << m_networkReply->errorString();
        // download failed
        emit downloadError(m_download->getId(), DownloadManager::UnknownError);
    } else
        emit downloadFinished();

    m_networkReply->deleteLater();
    m_networkReply = 0;
}

void DownloadThread::slotReadyRead()
{
    if (m_networkReply)
        m_output.write(m_networkReply->readAll());
}

int DownloadThread::getCurrentRemainTime()
{
    return m_currentRemainTime;
}

void DownloadThread::stop()
{
    disconnectSignals();
}

DownloadThread::~DownloadThread()
{
    m_networkAccessManager->deleteLater();

    if (m_downloadTime)
        delete m_downloadTime;
    m_downloadTime = 0;

    if (m_downloadProgressTimer)
        delete m_downloadProgressTimer;
    m_downloadProgressTimer = 0;

    if (m_downloadTimeoutTimer)
        delete m_downloadTimeoutTimer;
    m_downloadTimeoutTimer = 0;

    this->wait();
}
