/*!
 * \file downloadmanager.h
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QStringList>
#include <QSslError>

class DownloadManagerImpl;
class DownloadManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(UrlType)
    Q_ENUMS(DownloadStatus)
    Q_ENUMS(DownloadErrorCode)
public:
    enum UrlType {
        UnknownType = -1,
        Application = 0,
        Music = 1,
        Video = 2,
        Playlist = 3
    };

    enum DownloadStatus {
        UnknownStatus = -1,
        Queueing = 0,
        Downloading = 1,
        Pausing = 2,
        Stopping = 3,
        Error = 4,
        Finished = 5
    };

    enum DownloadErrorCode {
        UnknownError = -1,
        NoError = 0,

        // network layer errors [relating to the destination server] (1-99):
        ConnectionRefusedError = 1,
        RemoteHostClosedError = 2,
        HostNotFoundError = 3,
        TimeoutError = 4,
        OperationCanceledError = 5,
        SslHandshakeFailedError = 6,
        TemporaryNetworkFailureError = 7,
        UnknownNetworkError = 99,

        // proxy errors (101-199):
        ProxyConnectionRefusedError = 101,
        ProxyConnectionClosedError = 102,
        ProxyNotFoundError = 103,
        ProxyTimeoutError = 104,
        ProxyAuthenticationRequiredError = 105,
        UnknownProxyError = 199,

        // content errors (201-299):
        ContentAccessDenied = 201,
        ContentOperationNotPermittedError = 202,
        ContentNotFoundError = 203,
        AuthenticationRequiredError = 204,
        ContentReSendError = 205,
        UnknownContentError = 299,

        // protocol errors
        ProtocolUnknownError = 301,
        ProtocolInvalidOperationError = 302,
        ProtocolFailure = 399,

        // logic errors (401-499)
        UnknowDownloadId = 401,
        DownloadInQueue = 402,
        FileDownloading = 403,
        CanNotInsertDownloadToDB = 404, // Can not insert record set to db, might db has been being locked
        CanNotWriteToDisk = 405, // Can not write data to the disk
        UnknownUrlType = 499
    };

    explicit DownloadManager(QObject *parent = 0);
    ~DownloadManager();

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
    Q_INVOKABLE int getUrlTypeByUrl(const QString &url);

    /*!
     * \brief Get url type
     * \param contactId: id of the contact
     * \returns check results from UrlType
     */
    Q_INVOKABLE int getUrlTypeByContactId(int contactId);

    /*!
     * \brief Get url type
     * \param downloadId: id of the download
     * \returns check results from UrlType
     */
    Q_INVOKABLE int getUrlTypeByDownloadId(int downloadId);

    /*!
     * \brief Get status of download
     * \param downloadId: id of download from the download table
     * \returns check results from DownloadStatus
     */
    Q_INVOKABLE int getDownloadStatus(int downloadId);

    /*!
     * \brief Pause a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    Q_INVOKABLE bool pauseDownload(int downloadId);

    /*!
     * \brief Resume a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    Q_INVOKABLE bool resumeDownload(int downloadId);

    /*!
     * \brief Stop a download
     * \param downloadId: the id of the download
     * \returns true if successful, otherwise returns false
     * \note the downloadId is the id of download from the database
     */
    Q_INVOKABLE bool stopDownload(int downloadId);

    /*!
     * \brief QML calls this when a contact is removed to delete the download links this contact from the database
     * \param contactId: the id of the contact
     * \returns true if successful, otherwise returns false
     * \note the contactId is the id of contact from the database
     */
    Q_INVOKABLE bool deleteDownloadContact(int contactId);

    /*!
     * \brief set database path name of the app
     * \param dbPath: full file path name
     */
    Q_INVOKABLE void setDBStoragePath(const QString &dbPath);

    /*!
     * \brief get current remain time by download id
     * \returns current remain time
     * \returns -1 if it does not receive data
     */
    Q_INVOKABLE int getCurrentRemainTimeByDownload(int downloadId);

    /*!
     * \brief get current remain time by contact id
     * \returns current remain time
     * \returns -1 if it does not receive data
     */
    Q_INVOKABLE int getCurrentRemainTimeByContact(int contactId);

signals:

    /*!
     * \brief Add url to queue to download
     * \param url: url of the file to download
     * \param contactId: the id of the contact from database
     */
    void signalAddUrl(const QString &url, int contactId);

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

protected:

    // Start up the object
    void initialize();

    // Used for releasing pointers (Free memory)
    void release();

    // Connect signals
    void connectSignals();

    // Disconnect signals
    void disconnectSignals();

private:

    DownloadManagerImpl *m_downloadManagerImpl;

    // Separated to other thread
    QThread *m_downloadManagerThread;
};

#endif // DOWNLOADMANAGER_H
