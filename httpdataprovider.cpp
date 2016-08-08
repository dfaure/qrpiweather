/*  This file is part of qmeteofrance
 *  Copyright 2014  David Faure  <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "httpdataprovider.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <iostream>

HttpDataProvider::HttpDataProvider(const QString &cacheFile)
    : m_qnam(new QNetworkAccessManager(this)),
      m_cacheFile(cacheFile)
{
}

HttpDataProvider::~HttpDataProvider()
{
    qDebug() << "dtor HDP";
}

void HttpDataProvider::ensureDataAvailable()
{
    std::cerr << "Downloading weather data, please wait..." << std::endl;
    // Vedene is harcoded into the latitude/longitude
    QUrl url("http://www.infoclimat.fr/public-api/gfs/json?_ll=43.973595,4.917150&_auth=BhxRRlYoUHIEKQQzUiRVfFM7VWAIflB3BHgEZ186XiMHbANiVTVRN1A%2BAH0FKlZgWHVXNAw3UGAFblIqCnhePwZsUT1WPVA3BGsEYVJ9VX5TfVU0CChQdwRvBGtfLF48B2wDYFUoUTJQOABiBStWY1hsVzEMLFB3BWdSMQpgXj0GZVE9VjBQNQRrBGNSfVV%2BU2VVMgg%2BUD4ENQQyXzZeNAdtA2NVZVExUGsAawUrVmZYbFc0DDRQaAVgUjMKZl4iBnpRTFZGUC8EKwQkUjdVJ1N9VWAIaVA8&_c=54e0919e3cb6c5e964bf31c7c759a70b");
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    /*QNetworkReply* reply =*/ m_qnam->get(request);
    connect(m_qnam, &QNetworkAccessManager::finished, this, &HttpDataProvider::slotFinished);
}

void HttpDataProvider::slotFinished(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll(); // QJsonDocument can't read from a QIODevice !
    reply->deleteLater();
    if (data.isEmpty()) {
        qWarning() << "Empty document downloaded";
        emit error();
        return;
    }
    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (doc.isNull()) {
        qWarning() << "Error parsing JSON document:" << jsonError.errorString() << "at offset" << jsonError.offset;
        emit error();
        return;
    }
    // Write out cache
    QDir().mkpath(QFileInfo(m_cacheFile).absolutePath());
    QFile file(m_cacheFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
    }
    emit dataAvailable(doc);
}
