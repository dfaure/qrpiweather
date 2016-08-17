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

#include "openweathermapparser.h"
#include <KUnitConversion/Value>

#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>

OpenWeatherMapParser::OpenWeatherMapParser()
{
}

OpenWeatherMapParser::~OpenWeatherMapParser()
{
}

QUrl OpenWeatherMapParser::url() const
{
    // http://bulk.openweathermap.org/sample/city.list.json.gz says:
    // {"_id":6445119,"name":"Vedène","country":"FR","coord":{"lon":4.9,"lat":43.98333}}
    // {"_id":2970253,"name":"Vedene","country":"FR","coord":{"lon":4.90428,"lat":43.97744}}
    QUrl url("http://api.openweathermap.org/data/2.5/forecast?id=2970253&units=metric&appid=70b19951ab915a2ea046ac59279a601c");
    return url;
}

QString OpenWeatherMapParser::cacheFileName() const
{
    return "openweathermap.json";
}

QString OpenWeatherMapParser::backendName() const
{
    return QStringLiteral("openweathermap.org");
}

QVector<WeatherDataEntry> OpenWeatherMapParser::parse(const QByteArray &data)
{
    QVector<WeatherDataEntry> wdlist;
    if (data.startsWith("<html>")) { // error
        qWarning() << QString::fromLatin1(data);
        return wdlist;
    }

    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (doc.isNull()) {
        qWarning() << "Error parsing JSON document:" << jsonError.errorString() << "at offset" << jsonError.offset;
        return wdlist;
    }

#if 0
    QFile dump("dump.json");
    if (dump.open(QIODevice::WriteOnly)) {
        dump.write(doc.toJson(QJsonDocument::Indented));
    }
#endif
    // Docu at http://openweathermap.org/forecast5#JSON

    const QJsonObject root = doc.object();
    const QJsonArray respList = root.value("list").toArray();
    wdlist.reserve(respList.count());
    for (int i = 0; i < respList.count(); ++i) {
        const QJsonObject details = respList.at(i).toObject();
        const QString dateStr = details.value("dt_txt").toString();
        const QDateTime dateTime = QDateTime::fromString(dateStr, Qt::ISODate); // result is in LocalTime, which is fine
        Q_ASSERT(dateTime.isValid());
        const QJsonObject main = details.value("main").toObject();
        const double celsius = main.value("temp").toDouble();
        const QJsonObject windObject = details.value("wind").toObject();
        const int average_wind = qRound(windObject.value("speed").toDouble() * 3.6); // convert from m/s to km/h
        const int gust_wind = 0; // NOT AVAILABLE :(
        const int wind_direction = windObject.value("deg").toInt();
        const QJsonObject rainObject = details.value("rain").toObject();
        const double mm_rain = rainObject.value("3h").toDouble();
        const QJsonArray weatherArray = details.value("weather").toArray();
        const QJsonObject weatherObject = weatherArray.isEmpty() ? QJsonObject() : weatherArray.at(0).toObject();
        const QString iconName = weatherObject.value("icon").toString();

        // Should we display this?
        // qDebug() << weatherObject.value("description").toString();
        // English: clear sky, few clouds
        // France: ensoleillé, ensoleillé

        WeatherDataEntry wd;
        wd.setTemperatureWindRain(dateTime, celsius, average_wind, gust_wind, wind_direction, mm_rain);
        // see http://openweathermap.org/weather-conditions
        wd.setWeatherIcon("http://openweathermap.org/img/w/" + iconName + ".png");
        wdlist.append(wd);
        //qDebug() << dateTime << celsius << "vent" << average_wind << gust_wind << "direction" << wind_direction << "rain" << mm_rain;
    }
    return wdlist;
}
