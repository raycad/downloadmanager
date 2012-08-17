/*!
 * \file dbconnection.cpp
 * \brief database connection
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "dbconnection.h"
#include <QDir>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

DBConnection::DBConnection(QObject *parent) :
    QObject(parent)
{

}

void DBConnection::setDBFilePathName(const QString &dbFilePathName)
{
    m_databaseFilePathName = dbFilePathName;
}

bool DBConnection::open(const QString &dbFilePathName)
{
    if (dbFilePathName.isEmpty())
        return false;

    // If the current path is the same as the input path and the connection is openning then return true
    if ((dbFilePathName == m_databaseFilePathName) &&
            isOpen())
        return true;

    // If the current is openning, close the connection
    if (isOpen())
        m_sqlDatabase.close();

    m_sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
    m_sqlDatabase.setDatabaseName(dbFilePathName);
    bool ret = m_sqlDatabase.open();

    m_databaseFilePathName = dbFilePathName;

    return ret;
}

bool DBConnection::open()
{
    return open(m_databaseFilePathName);
}

bool DBConnection::isOpen()
{
    return m_sqlDatabase.isOpen();
}

bool DBConnection::isValid()
{
    return m_sqlDatabase.isValid();
}

bool DBConnection::isTableExisting(const QString &tableName)
{
    if (!m_sqlDatabase.isValid())
        return false;
    if (!m_sqlDatabase.isOpen())
        return false;

    QStringList tableList = m_sqlDatabase.tables(QSql::Tables);
    foreach (QString str, tableList) {
        if (tableName == str)
            return true;
    }

    return false;
}

bool DBConnection::createTable(const QString &queryString)
{
    QSqlQuery query(m_sqlDatabase);
    query.prepare(queryString);
    if (!query.exec()) {
        qDebug() << __PRETTY_FUNCTION__ << queryString << query.lastError().text();
        return false;
    }

    return true;
}

QSqlDatabase DBConnection::getSqlDatabase()
{
    return m_sqlDatabase;
}

void DBConnection::close()
{
    m_sqlDatabase.close();
}
