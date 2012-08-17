/*!
 * \file download.h
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QNetworkReply>
#include "contact.h"
#include "downloadmanager.h"

class DownloadThread;
class Download : public QObject
{
    Q_OBJECT
public:

    // Constructor
    explicit Download(QObject *parent = 0);
    // Destructor
    ~Download();

    /*!
     * \brief Set url for download
     * \param url: the url of the download
     */
    void setUrl(const QString &url);

    /*!
     * \brief Get url of download
     * \returns: the url of the download
     */
    QString getUrl();

    /*!
     * \brief Set id of the download
     * \param id: the id of the download
     */
    void setId(int id);

    /*!
     * \brief Get id of the download
     * \returns the id of the download
     */
    int getId();

    /*!
     * \brief Set contact linked of the download
     * \param contact: the linked contact
     */
    void setContact(Contact *contact);

    /*!
     * \brief Get the linked contact
     * \returns the linked contact
     */
    Contact *getContact();

    /*!
     * \brief Set download status of the download
     * \param downloadStatus: the status of the download
     */
    void setDownloadStatus(DownloadManager::DownloadStatus downloadStatus);

    /*!
     * \brief Get download status of the download
     * \returns the status of the download
     */
    DownloadManager::DownloadStatus getDownloadStatus();

    /*!
     * \brief Set url type of the download
     * \param urlType: url type of the download
     */
    void setUrlType(DownloadManager::UrlType urlType);

    /*!
     * \brief Get url type of the download
     * \returns the url type of the download
     */
    DownloadManager::UrlType getUrlType();

    /*!
     * \brief Get the saved file path name
     * \returns he saved file path name
     */
    QString getSavedFilePathName();

    /*!
     * \brief start download
     */
    void start();

    /*!
     * \brief pause download
     */
    void pause();

    /*!
     * \brief resume download
     */
    void resume();

    /*!
     * \brief stop download
     */
    void stop();

    /*!
     * \brief get current remain time
     * \returns current remain time
     */
    int getCurrentRemainTime();

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
    void downloadError(int downloadId, DownloadManager::DownloadErrorCode error);

    /*!
     * \brief emitted when an ssl error occurs
     * \param downloadId: id of download
     * \param errors: QSslError
     */
    void downloadSslErrors(int downloadId, const QList<QSslError> &errors);

protected slots:
    // Slot when download finished
    void slotDownloadFinished();

    // Slot when getting ssl errors
    void slotDownloadSslErrors(const QList<QSslError> &errors);

protected:

    // Connect signals
    void connectSignals();

    // Disconnect signals
    void disconnectSignals();

    /*!
     * \brief Save data to file
     * \param url: url to download
     */
    QString saveFileName(const QString &url);

    // Release pointers to avoid memory leak
    void release();

private:

    DownloadThread *m_downloadThread; // Separated download thread

    DownloadManager::DownloadStatus m_downloadStatus; // Download status
    DownloadManager::UrlType m_urlType; // Url type

    QString m_url; // Download Url
    int m_id; // Download Id
    Contact *m_contact; // The link contact, the download will manage the contact time life
    QString m_savedFilePathName; // Saved full file path name
};

#endif // DOWNLOAD_H
