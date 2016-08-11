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

HttpXmlDataProvider::HttpXmlDataProvider(const QString &cacheFile)
    : m_qnam(new QNetworkAccessManager(this)),
      m_cacheFile(cacheFile)
{
}

HttpXmlDataProvider::~HttpXmlDataProvider()
{
}

void HttpXmlDataProvider::ensureDataAvailable()
{
    std::cerr << "Downloading weather data, please wait..." << std::endl;
    // Vedene is harcoded into the URL

#if 0
    // plasma-workspace/dataengines/weather/ions/wetter.com/ion_wettercom.cpp

    // wetter.com API project data
    #define PROJECTNAME "weatherion"
    #define SEARCH_URL "http://api.wetter.com/location/index/search/%1/project/" PROJECTNAME "/cs/%2"
    #define FORECAST_URL "http://api.wetter.com/forecast/weather/city/%1/project/" PROJECTNAME "/cs/%2"
    #define APIKEY "07025b9a22b4febcf8e8ec3e6f1140e8"

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QString::fromLatin1(PROJECTNAME).toUtf8());
    md5.addData(QString::fromLatin1(APIKEY).toUtf8());
    md5.addData(m_place[source].placeCode.toUtf8());
    const QString encodedKey = QString::fromLatin1(md5.result().toHex());

    const QUrl url(QString::fromLatin1(FORECAST_URL).arg(m_place[source].placeCode, encodedKey));
#else
    // Result of the above, fetched from the plasmashell or kio debug output
    const QUrl url("http://api.wetter.com/forecast/weather/city/FRXY00507/project/weatherion/cs/95292d30da1a503b561ae10a3f90b0be");
#endif
    QNetworkRequest request(url);
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
