/*!
 * \file downloadmanager.cpp
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "downloadmanager.h"
#include "contact.h"
#include "dbconnection.h"
#include <QDebug>
#include "download.h"
#include "downloaddao.h"
#include "dbconnection.h"
#include "downloadmanagerimpl.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    initialize();
}

void DownloadManager::initialize()
{
    m_downloadManagerThread = new QThread();
    Q_ASSERT(m_downloadManagerThread != 0);
    m_downloadManagerThread->start();

    m_downloadManagerImpl = new DownloadManagerImpl();
    Q_ASSERT(m_downloadManagerImpl != 0);

    m_downloadManagerImpl->moveToThread(m_downloadManagerThread);

    connectSignals();
}

void DownloadManager::addUrl(const QString &url, int contactId)
{
    QMetaObject::invokeMethod(m_downloadManagerImpl, "addUrl", Qt::QueuedConnection, Q_ARG(QString, url), Q_ARG(int, contactId));
}

int DownloadManager::getUrlTypeByUrl(const QString &url)
{
    return m_downloadManagerImpl->getUrlTypeByUrl(url);
}

int DownloadManager::getUrlTypeByContactId(int contactId)
{
    return m_downloadManagerImpl->getUrlTypeByContactId(contactId);
}

int DownloadManager::getUrlTypeByDownloadId(int downloadId)
{
    return m_downloadManagerImpl->getUrlTypeByDownloadId(downloadId);
}

int DownloadManager::getDownloadStatus(int downloadId)
{
    return m_downloadManagerImpl->getDownloadStatus(downloadId);
}

bool DownloadManager::pauseDownload(int downloadId)
{
    return m_downloadManagerImpl->pauseDownload(downloadId);
}

bool DownloadManager::resumeDownload(int downloadId)
{
    return m_downloadManagerImpl->resumeDownload(downloadId);
}

bool DownloadManager::stopDownload(int downloadId)
{    
    return m_downloadManagerImpl->stopDownload(downloadId);
}

int DownloadManager::getCurrentRemainTimeByDownload(int downloadId)
{
    return m_downloadManagerImpl->getCurrentRemainTimeByDownload(downloadId);
}

int DownloadManager::getCurrentRemainTimeByContact(int contactId)
{
    return m_downloadManagerImpl->getCurrentRemainTimeByContact(contactId);
}

void DownloadManager::release()
{
    disconnectSignals();

    if (m_downloadManagerImpl) {
        delete m_downloadManagerImpl;
        m_downloadManagerImpl = 0;
    }

    if (m_downloadManagerThread) {
        m_downloadManagerThread->exit();
        delete m_downloadManagerThread;
        m_downloadManagerThread = 0;
    }
}

bool DownloadManager::deleteDownloadContact(int contactId)
{
    return m_downloadManagerImpl->deleteDownloadContact(contactId);
}

void DownloadManager::setDBStoragePath(const QString &dbPath)
{
    return m_downloadManagerImpl->setDBStoragePath(dbPath);
}

void DownloadManager::connectSignals()
{
    connect(m_downloadManagerImpl, SIGNAL(contactDownloadError(int,int)), this, SIGNAL(contactDownloadError(int,int)), Qt::DirectConnection);
    connect(m_downloadManagerImpl, SIGNAL(downloadError(int,int)), this, SIGNAL(downloadError(int,int)), Qt::DirectConnection);
    connect(m_downloadManagerImpl, SIGNAL(downloadFinished(int,int,QString,int)), this, SIGNAL(downloadFinished(int,int,QString,int)), Qt::DirectConnection);
    connect(m_downloadManagerImpl, SIGNAL(downloadStatusChanged(int,int,int)), this, SIGNAL(downloadStatusChanged(int,int,int)), Qt::DirectConnection);
    connect(m_downloadManagerImpl, SIGNAL(downloadTimeRemain(int,int)), this, SIGNAL(downloadTimeRemain(int,int)), Qt::DirectConnection);
    connect(m_downloadManagerImpl, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)), Qt::DirectConnection);
}

void DownloadManager::disconnectSignals()
{
    disconnect(m_downloadManagerImpl, SIGNAL(contactDownloadError(int,int)), this, SIGNAL(contactDownloadError(int,int)));
    disconnect(m_downloadManagerImpl, SIGNAL(downloadError(int,int)), this, SIGNAL(downloadError(int,int)));
    disconnect(m_downloadManagerImpl, SIGNAL(downloadFinished(int,int,QString,int)), this, SIGNAL(downloadFinished(int,int,QString,int)));
    disconnect(m_downloadManagerImpl, SIGNAL(downloadStatusChanged(int,int,int)), this, SIGNAL(downloadStatusChanged(int,int,int)));
    disconnect(m_downloadManagerImpl, SIGNAL(downloadTimeRemain(int,int)), this, SIGNAL(downloadTimeRemain(int,int)));
    disconnect(m_downloadManagerImpl, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)));
}

DownloadManager::~DownloadManager()
{
    release();
}
