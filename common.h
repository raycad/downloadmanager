/*!
 * \file common.h
 * \brief declare constants
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef COMMON_H
#define COMMON_H

#include <QString>

const int MAX_CONCURRENT_DOWNLOAD_THREADS = 3;

const QString SAVED_DOWNLOAD_DIRECTORY = "/.picphone/Downloads/";

const QString DOWNLOAD_TABLE_NAME = "download";

const int DOWNLOAD_PROGRESS_INTERVAL = 5; // interval to emit progress signal of a download thread calculated by second

const int DOWNLOAD_TIMEOUT = 90; // download timeout of a download thread calculated by second

#endif // COMMON_H
