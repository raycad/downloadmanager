/*!
 * \file downloadmanagerimpl.cpp
 * \brief Download Manager Implementation
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "downloadmanagerimpl.h"
#include "contact.h"
#include "dbconnection.h"
#include <QDebug>
#include "download.h"
#include "downloaddao.h"
#include "dbconnection.h"
#include "downloadmanager.h"

DownloadManagerImpl::DownloadManagerImpl(QObject *parent) :
    QObject(parent)
{
    m_dbConnection = 0;
    m_downloadDAO = 0;

    initialize();
}

void DownloadManagerImpl::initialize()
{
    // Initialize supported types
    m_supportedMusicTypeList << ".mp3" << ".ogg" << ".ogv";
    m_supportedVideoTypeList << ".avi";
    m_supportedApplicationTypeList << ".wgt" << ".img";

    m_dbConnection = new DBConnection();
    Q_ASSERT(m_dbConnection != 0);
    m_downloadDAO = new DownloadDAO(m_dbConnection);
    Q_ASSERT(m_downloadDAO != 0);
}

bool DownloadManagerImpl::isDownloadExistingInQueue(int contactId)
{
    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        Contact *contact = download->getContact();
        if (contact == 0)
            continue;
        if (contact->getId() == contactId)
            return true;
    }

    return false;
}

bool DownloadManagerImpl::isDownloadExistingInList(int contactId)
{
    Download *download = getDownloadByContactId(contactId);
    if (download != 0)
        return true;

    return false;
}

void DownloadManagerImpl::addUrl(const QString &url, int contactId)
{
    DownloadManager::UrlType urlType = (DownloadManager::UrlType)getUrlTypeByUrl(url);
    if (urlType == DownloadManager::UnknownType) {
        // Emits contact download error signal
        emit contactDownloadError(contactId, DownloadManager::UnknownUrlType);
        return;
    }

    if (isDownloadExistingInQueue(contactId))  {
        // Emits contact download error signal
        emit contactDownloadError(contactId, DownloadManager::DownloadInQueue);
        return;
    }

    if (isDownloadExistingInList(contactId))  {
        // Emits contact download error signal
        emit contactDownloadError(contactId, DownloadManager::FileDownloading);
        return;
    }

    // Create objects and add to the queue
    Contact *contact = new Contact();
    Q_ASSERT(contact != 0);
    contact->setId(contactId);

    DownloadManager::DownloadStatus downloadStatus = DownloadManager::Queueing;
    Download *download = new Download();
    Q_ASSERT(download != 0);
    download->setUrl(url);
    download->setDownloadStatus(downloadStatus);
    download->setUrlType(urlType);
    download->setContact(contact);

    m_mutexLocker.lock();
    // Add download to the queue
    m_downloadQueue.enqueue(download);
    m_mutexLocker.unlock();

    // Add params to the download table
    int downloadId = m_downloadDAO->addDownload(download);

    if (downloadId < 0) {
        emit contactDownloadError(contactId, DownloadManager::CanNotInsertDownloadToDB);
        return;
    }

    download->setId(downloadId);

    // Emit download status change signal
    emit downloadStatusChanged(contactId, downloadId, (int)downloadStatus);

    checkDownloadQueue();
}

Download *DownloadManagerImpl::getDownloadByContactId(int contactId)
{
    QMutexLocker locker(&m_mutexLocker);
    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        Contact *contact = download->getContact();
        if (contact == 0)
            continue;
        if (contact->getId() == contactId)
            return download;
    }

    foreach(Download *download, m_downloadingList) {
        if (download == 0)
            continue;
        Contact *contact = download->getContact();
        if (contact == 0)
            continue;
        if (contact->getId() == contactId)
            return download;
    }

    return 0;
}

Download *DownloadManagerImpl::getDownloadByDownloadId(int downloadId)
{
    QMutexLocker locker(&m_mutexLocker);
    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        if (download->getId() == downloadId)
            return download;
    }

    foreach(Download *download, m_downloadingList) {
        if (download == 0)
            continue;
        if (download->getId() == downloadId)
            return download;
    }

    return 0;
}

bool DownloadManagerImpl::removeDownloadById(int downloadId)
{
    QMutexLocker locker(&m_mutexLocker);
    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        if (download->getId() == downloadId) {
            m_downloadQueue.removeAt(i);
            // Free memory
            download->stop();
            download->deleteLater();
            return true;
        }
    }

    for (int i = 0; i < m_downloadingList.count(); i++) {
        Download *download = m_downloadingList.at(i);
        if (download == 0)
            continue;
        if (download->getId() == downloadId) {
            m_downloadingList.removeAt(i);
            // Free memory
            download->stop();
            download->deleteLater();
            return true;
        }
    }

    return false;
}

bool DownloadManagerImpl::removeDownloadByContactId(int contactId)
{
    QMutexLocker locker(&m_mutexLocker);
    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        Contact *contact = download->getContact();
        if (contact == 0)
            continue;
        if (contact->getId() == contactId) {
            m_downloadQueue.removeAt(i);
            // Free memory
            download->stop();
            download->deleteLater();
            return true;
        }
    }

    for (int i = 0; i < m_downloadingList.count(); i++) {
        Download *download = m_downloadingList.at(i);
        if (download == 0)
            continue;
        Contact *contact = download->getContact();
        if (contact == 0)
            continue;
        if (contact->getId() == contactId) {
            m_downloadingList.removeAt(i);
            // Free memory
            download->stop();
            download->deleteLater();
            return true;
        }
    }

    return false;
}

void DownloadManagerImpl::checkDownloadQueue()
{
    m_mutexLocker.lock();
    int queueCount = m_downloadQueue.count();
    int downloadingCount = m_downloadingList.count();
    m_mutexLocker.unlock();

    while (downloadingCount < MAX_CONCURRENT_DOWNLOAD_THREADS) {
        if (queueCount == 0)
            return;

        m_mutexLocker.lock();
        Download *download = m_downloadQueue.dequeue();
        // Add the download to the downloading list
        m_downloadingList.append(download);

        // Recalculate params
        queueCount = m_downloadQueue.count();
        downloadingCount = m_downloadingList.count();

        m_mutexLocker.unlock();

        DownloadManager::DownloadStatus downloadStatus = DownloadManager::Downloading;

        // Update status
        download->setDownloadStatus(downloadStatus);
        m_downloadDAO->updateDownload(download);

        // Connect signals
        connectDownloadSignals(download);

        // Start the download
        download->start();

        // Emit download status change signal
        emit downloadStatusChanged(download->getContact()->getId(), download->getId(), (int)downloadStatus);
    }
}

QString DownloadManagerImpl::getTextExtension(const QString &text)
{
    int index = text.lastIndexOf(".");
    if (index < 0)
        return "";
    return text.right(text.size()-index);
}

int DownloadManagerImpl::getUrlTypeByUrl(const QString &url)
{
    QString fileExtension = getTextExtension(url);
    if (fileExtension == "")
        return DownloadManager::UnknownType;

    if (m_supportedMusicTypeList.contains(fileExtension))
        return DownloadManager::Music;

    if (m_supportedVideoTypeList.contains(fileExtension))
        return DownloadManager::Video;

    if (m_supportedApplicationTypeList.contains(fileExtension))
        return DownloadManager::Application;

    return DownloadManager::UnknownType;
}

int DownloadManagerImpl::getUrlTypeByContactId(int contactId)
{
    Download *download = getDownloadByContactId(contactId);
    if (download)
        return download->getUrlType();

    // Look up from the database
    download = m_downloadDAO->getDownloadByContactId(contactId);
    int urlType = DownloadManager::UnknownType;
    if (download) {
        urlType = download->getUrlType();
        // release pointer
        delete download;
    }

    return urlType;
}

int DownloadManagerImpl::getUrlTypeByDownloadId(int downloadId)
{
    Download *download = getDownloadByDownloadId(downloadId);
    if (download)
        return download->getUrlType();

    // Look up from the database
    download = m_downloadDAO->getDownloadByDownloadId(downloadId);
    int urlType = DownloadManager::UnknownType;
    if (download) {
        urlType = download->getUrlType();
        // release pointer
        delete download;
    }

    return urlType;
}

int DownloadManagerImpl::getDownloadStatus(int downloadId)
{
    Download *download = getDownloadByDownloadId(downloadId);
    if (download)
        return download->getDownloadStatus();

    // Look up from the database
    download = m_downloadDAO->getDownloadByDownloadId(downloadId);
    int downloadStatus = DownloadManager::UnknownStatus;
    if (download) {
        downloadStatus = download->getDownloadStatus();
        // release pointer
        delete download;
    } else
        qDebug() << __PRETTY_FUNCTION__ << " Could not find the download with id = " << downloadId;

    return downloadStatus;
}

bool DownloadManagerImpl::pauseDownload(int downloadId)
{
    // Qt does not support
    Q_UNUSED(downloadId);
    return true;
}

bool DownloadManagerImpl::resumeDownload(int downloadId)
{
    // Qt does not support
    Q_UNUSED(downloadId);
    return true;
}

bool DownloadManagerImpl::stopDownload(int downloadId)
{
    return removeAndUpdateDownload(downloadId, DownloadManager::Stopping);
}

bool DownloadManagerImpl::removeAndUpdateDownload(int downloadId, DownloadManager::DownloadStatus status)
{
    Download *download = getDownloadByDownloadId(downloadId);
    if (!download) {
        qDebug() << "Could not find downloadId = " << downloadId;

        // check download
        checkDownloadQueue();

        return false;
    }

    // Update status
    download->setDownloadStatus(status);
    if (!m_downloadDAO->updateDownload(download))
        qDebug() << __PRETTY_FUNCTION__ << " Could not update information for download with id = " << downloadId;

    // Remove the download from the downloading list
    removeDownloadById(downloadId);

    qDebug() << "Remove the download with downloadId = " << downloadId;

    // check download
    checkDownloadQueue();

    return true;
}

void DownloadManagerImpl::slotDownloadFinished(int contactId, int downloadId, const QString &filePath, int type)
{
    emit downloadFinished(contactId, downloadId, filePath, type);
    removeAndUpdateDownload(downloadId, DownloadManager::Finished);
    qDebug() << __PRETTY_FUNCTION__ << " Emitted downloadFinished signal, contactId = "
             << contactId << ", downloadId = " << downloadId;
}

void DownloadManagerImpl::slotDownloadError(int downloadId, DownloadManager::DownloadErrorCode error)
{
    emit downloadError(downloadId, error);
    removeAndUpdateDownload(downloadId, DownloadManager::Error);
    qDebug() << __PRETTY_FUNCTION__ << " Emitted downloadError DownloadErrorCode signal" << ", downloadId = " << downloadId;
}

void DownloadManagerImpl::slotDownloadSslErrors(int downloadId, const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    emit downloadError(downloadId, DownloadManager::SslHandshakeFailedError);
    removeAndUpdateDownload(downloadId, DownloadManager::Error);
    qDebug() << __PRETTY_FUNCTION__ << " Emitted downloadError SslHandshakeFailedError signal" << ", downloadId = " << downloadId;
}

int DownloadManagerImpl::getCurrentRemainTimeByDownload(int downloadId)
{
    Download *download = getDownloadByDownloadId(downloadId);
    if (download)
        return download->getCurrentRemainTime();

    return -1;
}

int DownloadManagerImpl::getCurrentRemainTimeByContact(int contactId)
{
    Download *download = getDownloadByContactId(contactId);
    if (download)
        return download->getCurrentRemainTime();

    return -1;
}

void DownloadManagerImpl::release()
{
    m_mutexLocker.lock();
    // Free downloads in the downloading list
    foreach(Download *download, m_downloadingList) {
        if (download == 0)
            continue;
        download->stop();
        download->deleteLater();
        download = 0;
    }
    m_downloadingList.clear();

    for (int i = 0; i < m_downloadQueue.count(); i++) {
        Download *download = m_downloadQueue.at(i);
        if (download == 0)
            continue;
        download->stop();
        download->deleteLater();
        download = 0;
    }
    m_downloadQueue.clear();
    m_mutexLocker.unlock();

    if (m_dbConnection) {
        m_dbConnection->close();
        delete m_dbConnection;
        m_dbConnection = 0;
    }

    if (m_downloadDAO) {
        delete m_downloadDAO;
        m_downloadDAO = 0;
    }
}

void DownloadManagerImpl::connectDownloadSignals(Download *download)
{
    connect(download, SIGNAL(downloadTimeRemain(int,int)), this, SIGNAL(downloadTimeRemain(int,int)), Qt::DirectConnection);
    connect(download, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)), Qt::DirectConnection);
    connect(download, SIGNAL(downloadFinished(int,int,QString,int)), this, SLOT(slotDownloadFinished(int,int,QString,int)), Qt::DirectConnection);
    connect(download, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)), this, SLOT(slotDownloadError(int,DownloadManager::DownloadErrorCode)), Qt::DirectConnection);
    connect(download, SIGNAL(downloadSslErrors(int,QList<QSslError>)), this, SLOT(slotDownloadSslErrors(int,QList<QSslError>)), Qt::DirectConnection);
}

void DownloadManagerImpl::disconnectDownloadSignals(Download *download)
{
    disconnect(download, SIGNAL(downloadProgress(int,qint64,qint64)), this, SIGNAL(downloadProgress(int,qint64,qint64)));
    disconnect(download, SIGNAL(noReceivedData(int)), this, SIGNAL(noReceivedData(int)));
    disconnect(download, SIGNAL(downloadFinished(int,QString,int)), this, SLOT(slotDownloadFinished(int,QString,int)));
    disconnect(download, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)), this, SIGNAL(downloadError(int,DownloadManager::DownloadErrorCode)));
    disconnect(download, SIGNAL(downloadSslErrors(int,QList<QSslError>)), this, SLOT(slotDownloadSslErrors(int,QList<QSslError>)));
}

bool DownloadManagerImpl::deleteDownloadContact(int contactId)
{
    // Remove from the lists
    removeDownloadByContactId(contactId);

    // Remove from the database
    return m_downloadDAO->deleteDownloadContact(contactId);
}

void DownloadManagerImpl::setDBStoragePath(const QString &dbPath)
{
    if (m_dbConnection == 0)
        return;

    m_dbConnection->setDBFilePathName(dbPath);
    if (!m_dbConnection->open()) {
        qDebug() << "Could not open database";
        return;
    }

    // Create the download table
    QString createDownloadTableString = QString(
            "CREATE TABLE IF NOT EXISTS %1(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "contact_id INTEGER, url TEXT, url_type INTEGER, status INTEGER)").arg(DOWNLOAD_TABLE_NAME);
    m_dbConnection = new DBConnection();
    m_dbConnection->createTable(createDownloadTableString);
}

DownloadManagerImpl::~DownloadManagerImpl()
{
    release();
}
