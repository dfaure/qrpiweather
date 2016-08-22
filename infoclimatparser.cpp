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

#include "infoclimatparser.h"

#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

InfoClimatParser::InfoClimatParser()
{
}

InfoClimatParser::~InfoClimatParser()
{
}

QUrl InfoClimatParser::url() const
{
    // Vedene is harcoded into the latitude/longitude
    QUrl url("http://www.infoclimat.fr/public-api/gfs/json?_ll=43.973595,4.917150&_auth=BhxRRlYoUHIEKQQzUiRVfFM7VWAIflB3BHgEZ186XiMHbANiVTVRN1A%2BAH0FKlZgWHVXNAw3UGAFblIqCnhePwZsUT1WPVA3BGsEYVJ9VX5TfVU0CChQdwRvBGtfLF48B2wDYFUoUTJQOABiBStWY1hsVzEMLFB3BWdSMQpgXj0GZVE9VjBQNQRrBGNSfVV%2BU2VVMgg%2BUD4ENQQyXzZeNAdtA2NVZVExUGsAawUrVmZYbFc0DDRQaAVgUjMKZl4iBnpRTFZGUC8EKwQkUjdVJ1N9VWAIaVA8&_c=54e0919e3cb6c5e964bf31c7c759a70b");
    return url;
}

QString InfoClimatParser::cacheFileName() const
{
    return "infoclimat.json";
}

QString InfoClimatParser::backendName() const
{
    return QStringLiteral("infoclimat.fr");
}

QVector<WeatherDataEntry> InfoClimatParser::parse(const QByteArray &data)
{
    QVector<WeatherDataEntry> wdlist;
    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (doc.isNull()) {
        qWarning() << "Error parsing JSON document:" << jsonError.errorString() << "at offset" << jsonError.offset;
        return wdlist;
    }

    /*QFile dump("dump.json");
    if (dump.open(QIODevice::WriteOnly)) {
        dump.write(doc.toJson(QJsonDocument::Indented));
    }*/

    const QJsonObject root = doc.object();
    const QStringList keys = root.keys(); // already sorted
    static QString request_state = QStringLiteral("request_state");
    if (!keys.contains(request_state))
        return wdlist;
    const int stateValue = root.value(request_state).toInt();
    if (stateValue != 200) {
        qWarning() << "Got error: request_state=" << stateValue;
        return wdlist;
    }
    wdlist.reserve(keys.count());
    foreach (const QString &key, keys) {
        const QDateTime dateTime = QDateTime::fromString(key, Qt::ISODate); // result is in LocalTime, which is fine
        if (!dateTime.isValid()) // filter out the other keys like request_state, request_key, message, model_run, source
            continue;
        const QJsonObject details = root.value(key).toObject();
        const QJsonObject temperature = details.value("temperature").toObject();
        const double kelvin = temperature.value("sol").toDouble();
        const int celsius = qRound(kelvin - 273.15); // https://fr.wikipedia.org/wiki/Kelvin
        const int average_wind = qRound(details.value("vent_moyen").toObject().value("10m").toDouble());
        const int gust_wind = qRound(details.value("vent_rafales").toObject().value("10m").toDouble());
        const int wind_direction = details.value("vent_direction").toObject().value("10m").toInt();
        const double mm_rain = details.value("pluie").toDouble();
        WeatherDataEntry wd;
        wd.setTemperatureWindRain(dateTime, celsius, average_wind, gust_wind, wind_direction, mm_rain);
        wdlist.append(wd);
        //qDebug() << dateTime << celsius << "vent" << average_wind << gust_wind << "direction" << wind_direction << "rain" << mm_rain;
    }
    return wdlist;
}
