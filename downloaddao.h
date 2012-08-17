/*!
 * \file downloaddao.h
 * \brief database helper for download entity
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DOWNLOADDAO_H
#define DOWNLOADDAO_H

#include <QObject>
#include "dbconnection.h"

class Download;
class DownloadDAO : public QObject
{
    Q_OBJECT
public:
    explicit DownloadDAO(DBConnection *dbConnection, QObject *parent = 0);

    /*!
     * \brief Check the contact download if it is existing
     * \param contactId: contact id of the download
     * \returns the id of the contact if it is successful, otherwise returns -1
     */
    int checkExistingContactDownload(int contactId);

    /*!
     * \brief Get download by download id
     * \param downloadId: the id of the download
     * \returns download object
     * \note The pointer later needs to be release to avoid memory leak
     */
    Download *getDownloadByDownloadId(int downloadId);

    /*!
     * \brief Get download by contact id
     * \param contactId: the id of the contact
     * \returns download object
     * \note The pointer later needs to be release to avoid memory leak
     */
    Download *getDownloadByContactId(int contactId);

    /*!
     * \brief Add a download to the database
     * \param download: download entity
     * \returns -1 if failed, otherwise returns the id of the download record
     */
    int addDownload(Download *download);

    /*!
     * \brief Insert a download to the database
     * \param download: download entity
     * \returns -1 if failed, otherwise returns the id of the download record
     */
    int insertDownload(Download *download);

    /*!
     * \brief Update a download to the database
     * \param download: download entity
     * \returns true if successful, otherwise returns false
     */
    bool updateDownload(Download *download);

    /*!
     * \brief Delete a download from the database
     * \param download: download entity
     * \returns true if successful, otherwise returns false
     */
    bool deleteDownload(Download *download);

    /*!
     * \brief Delete a download from the database
     * \param contactId: id contact of the download
     * \returns true if successful, otherwise returns false
     */
    bool deleteDownloadContact(int contactId);

protected:
    // Initialize class
    void initialize();

    bool checkDB();

private:
    DBConnection *m_dbConnection; // Database connection
};

#endif // DOWNLOADDAO_H
