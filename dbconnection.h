/*!
 * \file dbconnection.h
 * \brief database connection
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QSqlDatabase>

class DBConnection : public QObject
{
    Q_OBJECT
public:
    explicit DBConnection(QObject *parent = 0);

    /*!
     * \brief Set database file path name
     * \param dbFilePathName: the file path name of the database
     */
    void setDBFilePathName(const QString &dbFilePathName);

    /*!
     * \brief Open a connection
     * \param dbFilePathName: the file path name of the database
     */
    bool open(const QString &dbFilePathName);

    /*!
     * \brief Open a connection with the current database file path name
     */
    bool open();

    /*!
     * \brief Close the cuurent connection
     */
    void close();

    /*!
     * \brief Check if the table name is existing in the database
     * \param tableName: a table name
     */
    bool isTableExisting(const QString &tableName);

    /*!
     * \brief Check if the database is openning
     */
    bool isOpen();

    /*!
     * \brief Check if the database is valid
     */
    bool isValid();

    /*!
     * \brief Create a table from a query
     * \param queryString: a query string
     */
    bool createTable(const QString &queryString);

    /*!
     * \brief Get the SqlDatabase
     */
    QSqlDatabase getSqlDatabase();

private:

    QSqlDatabase m_sqlDatabase; // Qt sql database
    QString m_databaseFilePathName; // Database file path name
};

#endif // DBCONNECTION_H
