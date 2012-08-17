/*!
 * \file download.cpp
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "download.h"
#include "downloadmanager.h"
#include "downloadthread.h"
#include <QDir>

Download::Download(QObject *parent) :
    QObject(parent)
{
    m_id = -1;
    m_url = "";
    m_savedFilePathName = "";
    m_contact = 0;
    m_downloadThread = 0;
}

void Download::setUrl(const QString &url)
{
    m_url = url;
}

QString Download::getUrl()
{
    return m_url;
}

void Download::setId(int id)
{
    m_id = id;
}

int Download::getId()
{
    return m_id;
}

void Download::setContact(Contact *contact)
{
    // Free the last memory
    if (m_contact != 0)
        delete m_contact;

    m_contact = contact;
}

Contact *Download::getContact()
{
    return m_contact;
}

void Download::setDownloadStatus(DownloadManager::DownloadStatus downloadStatus)
{
    m_downloadStatus = downloadStatus;
}

DownloadManager::DownloadStatus Download::getDownloadStatus()
{
    return m_downloadStatus;
}

void Download::setUrlType(DownloadManager::UrlType urlType)
{
    m_urlType = urlType;
}

DownloadManager::UrlType Download::getUrlType()
{
    return m_urlType;
}

QString Download::getSavedFilePathName()
{
    return m_savedFilePathName;
}

void Download::connectSignals()
{
    if (m_downloadThread == 0)
        return;

    connect(m_downloadThread, SIGNAL(downloadTimeRemain(int,int)), this, SIGNAL(downloadTimeRemain(int,int)), Qt::DirectConnection);
    connect(m_downloadThread, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)), Qt::DirectConnection);
    connect(m_downloadThread, SIGNAL(downloadFinished()), this, SLOT(slotDownloadFinished()), Qt::DirectConnection);
    connect(m_downloadThread, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)), this, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)), Qt::DirectConnection);
    connect(m_downloadThread, SIGNAL(downloadSslErrors(QList<QSslError>)), this, SLOT(slotDownloadSslErrors(QList<QSslError>)), Qt::DirectConnection);
}

void Download::disconnectSignals()
{
    if (m_downloadThread == 0)
        return;

    disconnect(m_downloadThread, SIGNAL(downloadTimeRemain(int,int)), this, SIGNAL(downloadTimeRemain(int,int)));
    disconnect(m_downloadThread, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)));
    disconnect(m_downloadThread, SIGNAL(downloadFinished()), this, SLOT(slotDownloadFinished()));
    disconnect(m_downloadThread, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)), this, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)));
    disconnect(m_downloadThread, SIGNAL(downloadSslErrors(QList<QSslError>)), this, SLOT(slotDownloadSslErrors(QList<QSslError>)));
}

void Download::slotDownloadFinished()
{
    int contactId = -1;
    if (m_contact)
        contactId = m_contact->getId();

    emit downloadFinished(contactId, m_id, m_savedFilePathName, m_urlType);
}

void Download::slotDownloadSslErrors(const QList<QSslError> &errors)
{
    emit downloadSslErrors(m_id, errors);
}

QString Download::saveFileName(const QString &url)
{
    QDir saveDirectory(QDir::homePath());
    QString downloadDir = QDir::homePath() + SAVED_DOWNLOAD_DIRECTORY;
    saveDirectory.mkpath(downloadDir);

    QString contactIdStr = "";
    if (m_contact)
        contactIdStr = QString("%1").arg(m_contact->getId());

    QString basename = QString("Contact%1_%2").arg(contactIdStr, QFileInfo(url).fileName());
    QString downloadFilePath = downloadDir + basename;

    return downloadFilePath;
}

void Download::start()
{
    if (m_downloadThread != 0) {
        disconnectSignals();
        m_downloadThread->exit();
        delete m_downloadThread;
    }

    // Set saved file path name
    m_savedFilePathName = saveFileName(m_url);

    m_downloadThread = new DownloadThread(this);
    connectSignals();
    m_downloadThread->start();
}

void Download::pause()
{
    // TODO
}

void Download::resume()
{
    // TODO
}

void Download::stop()
{
    // TODO
    if (m_downloadThread) {
        m_downloadThread->stop();
    }
}

int Download::getCurrentRemainTime()
{
    return m_downloadThread->getCurrentRemainTime();
}

void Download::release()
{
    stop();

    disconnectSignals();

    if (m_contact != 0) {
        delete m_contact;
        m_contact = 0;
    }

    if (m_downloadThread != 0) {
        m_downloadThread->exit();
        delete m_downloadThread;
    }
}

Download::~Download()
{
    release();
}
