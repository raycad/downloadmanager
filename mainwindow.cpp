#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initialize()
{
    //ui->urlLineEdit->setText("http://01audiovideo.free.fr/ogg/danahilliot_why_did_gods_leave_us.ogg");
    ui->urlLineEdit->setText("http://localhost/test.img");
    ui->contactIdLineEdit->setText("1");

    QString dbPath = qApp->applicationDirPath() + "/downloadmanager.sqlite";
    m_downloadManager.setDBStoragePath(dbPath);

    connect(&m_downloadManager, SIGNAL(downloadError(int,int)), this, SLOT(onDownloadError(int,int)), Qt::DirectConnection);
    connect(&m_downloadManager, SIGNAL(contactDownloadError(int,int)), this, SLOT(onContactDownloadError(int,int)), Qt::DirectConnection);
    connect(&m_downloadManager, SIGNAL(downloadFinished(int,int,QString,int)), this, SLOT(onDownloadFinished(int,int,QString,int)), Qt::DirectConnection);
    connect(&m_downloadManager, SIGNAL(downloadStatusChanged(int,int,int)), this, SLOT(onDownloadStatusChanged(int,int,int)), Qt::DirectConnection);
    connect(&m_downloadManager, SIGNAL(downloadTimeRemain(int,int)), this, SLOT(onDownloadTimeRemain(int,int)), Qt::DirectConnection);
    connect(&m_downloadManager, SIGNAL(noReceivedData(int)), this, SLOT(onNoReceivedData(int)), Qt::DirectConnection);
}

void MainWindow::onDownloadTimeRemain(int downloadId, int remainTime)
{
    qDebug() << __PRETTY_FUNCTION__ << "downloadId = " << downloadId << ", remainTime = " << remainTime;
}

void MainWindow::onNoReceivedData(int downloadId)
{
    qDebug() << __PRETTY_FUNCTION__ << "downloadId = " << downloadId;
}

void MainWindow::onDownloadFinished(int contactId, int downloadId, const QString &filePath, int urlType)
{
    qDebug() << __PRETTY_FUNCTION__ << "downloadId = " << downloadId << ", contactId = " << contactId;
}

void MainWindow::onDownloadError(int downloadId, int errorCode)
{
    qDebug() << __PRETTY_FUNCTION__ << "downloadId = " << downloadId << ", errorCode = " << errorCode;
}

void MainWindow::onContactDownloadError(int contactId, int errorCode)
{
    qDebug() << __PRETTY_FUNCTION__ << "contactId = " << contactId << ", errorCode = " << errorCode;
}

void MainWindow::onDownloadStatusChanged(int contactId, int downloadId, int downloadStatus)
{
    qDebug() << __PRETTY_FUNCTION__ << "downloadId = " << downloadId << ", contactId = " << contactId
                << ", downloadStatus = " << downloadStatus;

    if (downloadStatus == 1) {
        QString downloadIdString = QString("%1").arg(downloadId);
        ui->downloadIdLineEdit->setText(downloadIdString);
    }
}

void MainWindow::on_addUrlPushButton_clicked()
{
    int contactId = ui->contactIdLineEdit->text().toInt();
    m_downloadManager.addUrl(ui->urlLineEdit->text(), contactId);
}

void MainWindow::on_stopDownloadPushButton_clicked()
{
    m_downloadManager.stopDownload(ui->downloadIdLineEdit->text().toInt());
}
