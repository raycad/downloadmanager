/*!
 * \file downloaddao.cpp
 * \brief database helper for download entity
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "downloaddao.h"
#include "download.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

DownloadDAO::DownloadDAO(DBConnection *dbConnection, QObject *parent) :
    QObject(parent), m_dbConnection(dbConnection)
{
    initialize();
}

void DownloadDAO::initialize()
{
}

bool DownloadDAO::checkDB()
{
    if (!m_dbConnection) {
        qDebug() << __PRETTY_FUNCTION__ << ". Lost Database Connection";
        return false;
    }

    /*if (!m_dbConnection->isOpen()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database is closed";
        return false;
    }*/

    return true;
}

int DownloadDAO::checkExistingContactDownload(int contactId)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return -1;
    }

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("SELECT id FROM %1 WHERE contact_id = :contact_id").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery selectQuery(db);
    selectQuery.prepare(queryStr);
    selectQuery.bindValue(":contact_id", contactId);

    if (!selectQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << selectQuery.lastError();
        return -1;
    }

    if(selectQuery.next()) {
       int id = selectQuery.value(0).toInt();
       return id;
    }

    return -1;
}

Download *DownloadDAO::getDownloadByDownloadId(int downloadId)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return 0;
    }

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("SELECT contact_id, url, url_type, status FROM %1 WHERE id = :id").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery selectQuery(db);
    selectQuery.prepare(queryStr);
    selectQuery.bindValue(":id", downloadId);

    if (!selectQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << selectQuery.lastError();
        return 0;
    }

    if(selectQuery.next()) {
        Download *download = new Download;
        Q_ASSERT(download != 0);
        Contact *contact = new Contact;
        Q_ASSERT(contact != 0);
        int intVal = selectQuery.value(0).toInt();
        contact->setId(intVal);
        download->setContact(contact);
        download->setId(downloadId);
        QString strVal = selectQuery.value(1).toString();
        download->setUrl(strVal);
        intVal = selectQuery.value(2).toInt();
        download->setUrlType((DownloadManager::UrlType)intVal);
        intVal = selectQuery.value(3).toInt();
        download->setDownloadStatus((DownloadManager::DownloadStatus)intVal);
        return download;
    }

    return 0;
}

Download *DownloadDAO::getDownloadByContactId(int contactId)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return 0;
    }

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("SELECT id, url, url_type, status FROM %1 WHERE contact_id = :contact_id").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery selectQuery(db);
    selectQuery.prepare(queryStr);
    selectQuery.bindValue(":contact_id", contactId);

    if (!selectQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << selectQuery.lastError();
        return 0;
    }

    if(selectQuery.next()) {
        Download *download = new Download;
        Q_ASSERT(download != 0);
        Contact *contact = new Contact;
        Q_ASSERT(contact != 0);
        contact->setId(contactId);
        download->setContact(contact);
        int intVal = selectQuery.value(0).toInt();
        download->setId(intVal);
        QString strVal = selectQuery.value(1).toString();
        download->setUrl(strVal);
        intVal = selectQuery.value(2).toInt();
        download->setUrlType((DownloadManager::UrlType)intVal);
        intVal = selectQuery.value(3).toInt();
        download->setDownloadStatus((DownloadManager::DownloadStatus)intVal);
        return download;
    }

    return 0;
}

int DownloadDAO::addDownload(Download *download)
{
    if (!download)
        return -1;

    Contact *contact = download->getContact();
    if (!contact)
        return -1;

    int contactId = -1;
    contactId = contact->getId();

    int id = checkExistingContactDownload(contactId);
    if (id == -1)
        id = insertDownload(download);
    else
        updateDownload(download);

    return id;
}

int DownloadDAO::insertDownload(Download *download)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return -1;
    }

    if (!download)
        return -1;

    int contactId = -1;
    Contact *contact = download->getContact();
    if (contact)
        contactId = contact->getId();

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("INSERT INTO %1(contact_id, url, url_type, status) VALUES(:contact_id, :url, :url_type, :status)").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery insertQuery(db);
    insertQuery.prepare(queryStr);
    insertQuery.bindValue(":contact_id", contactId);
    insertQuery.bindValue(":url", download->getUrl());
    insertQuery.bindValue(":url_type", download->getUrlType());
    insertQuery.bindValue(":status", download->getDownloadStatus());

    if (!insertQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << insertQuery.lastError();
        return -1;
    }

    return insertQuery.lastInsertId().toInt();
}

bool DownloadDAO::updateDownload(Download *download)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return false;
    }

    if (!download)
        return false;

    int contactId = -1;
    Contact *contact = download->getContact();
    if (contact)
        contactId = contact->getId();

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("UPDATE %1 SET url = :url, url_type = :url_type, status = :status WHERE contact_id = :contact_id").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery updateQuery(db);
    updateQuery.prepare(queryStr);
    updateQuery.bindValue(":contact_id", contactId);
    updateQuery.bindValue(":url", download->getUrl());
    updateQuery.bindValue(":url_type", download->getUrlType());
    updateQuery.bindValue(":status", download->getDownloadStatus());

    if (!updateQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << updateQuery.lastError();
        return false;
    }

    return true;
}

bool DownloadDAO::deleteDownload(Download *download)
{
    if (!download)
        return false;

    int contactId = -1;
    Contact *contact = download->getContact();
    if (!contact)
        return false;

    contactId = contact->getId();

    return deleteDownloadContact(contactId);
}

bool DownloadDAO::deleteDownloadContact(int contactId)
{
    if (!checkDB()) {
        qDebug() << __PRETTY_FUNCTION__ << ". Database Error";
        return false;
    }

    QSqlQuery q(m_dbConnection->getSqlDatabase());
    QString queryStr = QString("DELETE FROM %1 WHERE contact_id = :contact_id").arg(DOWNLOAD_TABLE_NAME);
    QSqlDatabase db = m_dbConnection->getSqlDatabase();
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare(queryStr);
    deleteQuery.bindValue(":contact_id", contactId);
    if (!deleteQuery.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << deleteQuery.lastError();
        return false;
    }

    return true;
}
