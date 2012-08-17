/*!
 * \file downloadmanagerimpl.h
 * \brief Download Manager Implementation
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DOWNLOADMANAGERIMPL_H
#define DOWNLOADMANAGERIMPL_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QQueue>
#include <QMutex>
#include <QSslError>
#include "downloadmanager.h"

class Download;
class DownloadDAO;
class DBConnection;
class DownloadManager;
class DownloadManagerImpl : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManagerImpl(QObject *parent = 0);
    ~DownloadManagerImpl();

    /*!
     * \brief Add url to queue to download
     * \param url: url of the file to download
     * \param contactId: the id of the contact from database
     */
    Q_INVOKABLE void addUrl(const QString &url, int contactId);

    /*!
     * \brief Get url type
     * \param url: url of the file
     * \returns check results from UrlType
     */
    int getUrlTypeByUrl(const QString &url);

    /*!
     * \brief Get url type
     * \param contactId: id of the contact
     * \returns check results from UrlType
     */
    int getUrlTypeByContactId(int contactId);

    /*!
     * \brief Get url type
     * \param downloadId: id of the download
     * \returns check results from UrlType
     */
    int getUrlTypeByDownloadId(int downloadId);

    /*!
     * \brief Get status of download
     * \param downloadId: id of download from the download table
     * \returns check results from DownloadStatus
     */
    int getDownloadStatus(int downloadId);

    /*!
     * \brief Pause a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    bool pauseDownload(int downloadId);

    /*!
     * \brief Resume a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    bool resumeDownload(int downloadId);

    /*!
     * \brief Stop a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    bool stopDownload(int downloadId);

    /*!
     * \brief QML calls this when a contact is removed to delete the download links this contact from the database
     * \param contactId: the id of the contact
     * \returns true if successful, otherwise returns false
     * \note the contactId is the id of contact from the database
     */
    bool deleteDownloadContact(int contactId);

    /*!
     * \brief set database path name of the app
     * \param dbPath: full file path name
     */
    void setDBStoragePath(const QString &dbPath);

    /*!
     * \brief get current remain time by download id
     * \returns current remain time
     * \returns -1 if it does not receive data
     */
    int getCurrentRemainTimeByDownload(int downloadId);

    /*!
     * \brief get current remain time by contact id
     * \returns current remain time
     * \returns -1 if it does not receive data
     */
    int getCurrentRemainTimeByContact(int contactId);

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
     * \param contactId: the id of the contact
     * \param downloadId: id of download
     * \param filePath: full path of download file saved
     * \param urlType: type of download url
     */
    void downloadFinished(int contactId, int downloadId, const QString &filePath, int urlType);

    /*!
     * \brief emitted when an error occurs
     * \param downloadId: id of download
     * \param errorCode: look up the results from DownloadErrorCode
     */
    void downloadError(int downloadId, int errorCode);

    /*!
     * \brief emitted when an error occurs
     * \param contactId: id of contact
     * \param errorCode: look up the results from DownloadErrorCode
     */
    void contactDownloadError(int contactId, int errorCode);

    /*!
     * \brief emitted when a download status changed
     * \param contactId: the id of the contact
     * \param downloadId: id of download
     * \param downloadStatus: the new download status
     */
    void downloadStatusChanged(int contactId, int downloadId, int downloadStatus);

public slots:

    /*!
     * \brief Slot when a download finished
     * \param downloadId: the id of the download
     * \param contactId: the id of the contact
     * \param filePath: the file path
     * \param type: the type of the url
     */
    void slotDownloadFinished(int contactId, int downloadId, const QString &filePath, int type);

    /*!
     * \brief Slot when an error occurs
     * \param downloadId: the id of the download
     * \param error: error
     */
    void slotDownloadError(int downloadId, DownloadManager::DownloadErrorCode error);

#ifndef QT_NO_OPENSSL
    /*!
     * \brief Slot when an ssl error occurs
     * \param downloadId: the id of the download
     * \param errors: errors
     */
    void slotDownloadSslErrors(int downloadId, const QList<QSslError> &errors);
#endif

protected:

    // Start up the object
    void initialize();

    // Used for releasing pointers (Free memory)
    void release();

    // Check if the download is in the queue
    bool isDownloadExistingInQueue(int contactId);
    // Check if the download is in downloading list
    bool isDownloadExistingInList(int contactId);

    /*!
     * \brief Get download by contact id
     * \param contactId: the id of the contact
     * \returns download entity
     */
    Download *getDownloadByContactId(int contactId);

    /*!
     * \brief Get download by download id
     * \param downloadId: the id of the download
     * \returns download entity
     */
    Download *getDownloadByDownloadId(int downloadId);

    // Remove on the lists only
    bool removeDownloadById(int downloadId);
    // Remove on the lists only
    bool removeDownloadByContactId(int contactId);

    // Remove download from the lists and update to the database
    bool removeAndUpdateDownload(int downloadId, DownloadManager::DownloadStatus status);

    // Check download in the queue to start
    void checkDownloadQueue();

    // Connect signals with a download
    void connectDownloadSignals(Download *download);

    // Disconnect signals with a download
    void disconnectDownloadSignals(Download *download);

    // Get text extension of an input text, returns empty if does not have
    QString getTextExtension(const QString &text);

private:

    // Waiting download queue
    QQueue<Download *> m_downloadQueue;
    // List of downloading object
    QList<Download *> m_downloadingList;

    // Supported meta type lists
    QStringList m_supportedMusicTypeList;
    QStringList m_supportedVideoTypeList;
    QStringList m_supportedApplicationTypeList;

    // Database connection
    DBConnection *m_dbConnection;
    // Database helper
    DownloadDAO *m_downloadDAO;

    QMutex m_mutexLocker; // mutex loker for synchronization
};

#endif // DOWNLOADMANAGERIMPL_H
