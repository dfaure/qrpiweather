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

#include "httpxmldataprovider.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <iostream>

HttpXmlDataProvider::HttpXmlDataProvider(const QUrl &url, const QString &cacheFile)
    : m_qnam(new QNetworkAccessManager(this)),
      m_cacheFile(cacheFile),
      m_url(url)
{
}

HttpXmlDataProvider::~HttpXmlDataProvider()
{
}

void HttpXmlDataProvider::ensureDataAvailable()
{
    std::cerr << "Downloading weather data, please wait..." << std::endl;

    QNetworkRequest request(m_url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    /*QNetworkReply* reply =*/ m_qnam->get(request);
    connect(m_qnam, &QNetworkAccessManager::finished, this, &HttpXmlDataProvider::slotFinished);
}

void HttpXmlDataProvider::slotFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    reply->deleteLater();
    // Write out cache
    QDir().mkpath(QFileInfo(m_cacheFile).absolutePath());
    QFile file(m_cacheFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
    }
    emit dataAvailable(data);
}
