#include "wettercomparser.h"

#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QXmlStreamReader>

WetterComParser::WetterComParser(QTextStream &stream)
    : m_errorStream(stream)
{
}

WetterComParser::~WetterComParser()
{
}

static QMap<QString, QString> setupCommonIconMappings()
{
    QMap<QString, QString> conditionIcons;

    conditionIcons.insert(QStringLiteral("3"),  QStringLiteral("weather-many-clouds"));
    conditionIcons.insert(QStringLiteral("30"), QStringLiteral("weather-many-clouds"));
    conditionIcons.insert(QStringLiteral("4"),  QStringLiteral("weather-mist")); // no specific icon for Haze
    conditionIcons.insert(QStringLiteral("40"), QStringLiteral("weather-mist")); // "
    conditionIcons.insert(QStringLiteral("45"), QStringLiteral("weather-mist")); // "
    conditionIcons.insert(QStringLiteral("48"), QStringLiteral("weather-mist")); // "
    conditionIcons.insert(QStringLiteral("49"), QStringLiteral("weather-mist")); // "
    conditionIcons.insert(QStringLiteral("5"),  QStringLiteral("weather-mist"));
    conditionIcons.insert(QStringLiteral("50"), QStringLiteral("weather-mist"));
    conditionIcons.insert(QStringLiteral("51"), QStringLiteral("weather-mist"));
    conditionIcons.insert(QStringLiteral("53"), QStringLiteral("weather-mist"));
    conditionIcons.insert(QStringLiteral("55"), QStringLiteral("weather-mist"));
    conditionIcons.insert(QStringLiteral("56"), QStringLiteral("weather-freezing-rain"));
    conditionIcons.insert(QStringLiteral("57"), QStringLiteral("weather-freezing-rain"));
    conditionIcons.insert(QStringLiteral("6"),  QStringLiteral("weather-showers"));
    conditionIcons.insert(QStringLiteral("60"), QStringLiteral("weather-showers-scattered"));
    conditionIcons.insert(QStringLiteral("61"), QStringLiteral("weather-showers-scattered"));
    conditionIcons.insert(QStringLiteral("63"), QStringLiteral("weather-showers"));
    conditionIcons.insert(QStringLiteral("65"), QStringLiteral("weather-showers"));
    conditionIcons.insert(QStringLiteral("66"), QStringLiteral("weather-freezing-rain"));
    conditionIcons.insert(QStringLiteral("67"), QStringLiteral("weather-freezing-rain"));
    conditionIcons.insert(QStringLiteral("68"), QStringLiteral("weather-snow-rain"));
    conditionIcons.insert(QStringLiteral("69"), QStringLiteral("weather-snow-rain"));
    conditionIcons.insert(QStringLiteral("7"),  QStringLiteral("weather-snow"));
    conditionIcons.insert(QStringLiteral("70"), QStringLiteral("weather-snow-scattered"));
    conditionIcons.insert(QStringLiteral("71"), QStringLiteral("weather-snow-scattered"));
    conditionIcons.insert(QStringLiteral("73"), QStringLiteral("weather-snow"));
    conditionIcons.insert(QStringLiteral("75"), QStringLiteral("weather-snow-scattered"));
    conditionIcons.insert(QStringLiteral("8"),  QStringLiteral("weather-showers-scattered"));
    conditionIcons.insert(QStringLiteral("81"), QStringLiteral("weather-showers-scattered"));
    conditionIcons.insert(QStringLiteral("82"), QStringLiteral("weather-showers-scattered"));
    conditionIcons.insert(QStringLiteral("83"), QStringLiteral("weather-snow-rain"));
    conditionIcons.insert(QStringLiteral("84"), QStringLiteral("weather-snow-rain"));
    conditionIcons.insert(QStringLiteral("85"), QStringLiteral("weather-snow"));
    conditionIcons.insert(QStringLiteral("86"), QStringLiteral("weather-snow"));
    conditionIcons.insert(QStringLiteral("9"),  QStringLiteral("weather-storm"));
    conditionIcons.insert(QStringLiteral("90"), QStringLiteral("weather-storm"));
    conditionIcons.insert(QStringLiteral("96"), QStringLiteral("weather-storm"));
    conditionIcons.insert(QStringLiteral("999"), QStringLiteral("weather-none-available"));

    return conditionIcons;
}

static QMap<QString, QString> setupDayIconMappings()
{
    QMap<QString, QString> conditionList = setupCommonIconMappings();

    conditionList.insert(QStringLiteral("0"),  QStringLiteral("weather-clear"));
    conditionList.insert(QStringLiteral("1"),  QStringLiteral("weather-few-clouds"));
    conditionList.insert(QStringLiteral("10"), QStringLiteral("weather-few-clouds"));
    conditionList.insert(QStringLiteral("2"),  QStringLiteral("weather-clouds"));
    conditionList.insert(QStringLiteral("20"), QStringLiteral("weather-clouds"));
    conditionList.insert(QStringLiteral("80"), QStringLiteral("weather-showers-scattered-day"));
    conditionList.insert(QStringLiteral("95"), QStringLiteral("weather-storm-day"));

    return conditionList;
}

static QMap<QString, QString> setupNightIconMappings()
{
    QMap<QString, QString> conditionList = setupCommonIconMappings();

    conditionList.insert(QStringLiteral("0"),  QStringLiteral("weather-clear-night"));
    conditionList.insert(QStringLiteral("1"),  QStringLiteral("weather-few-clouds-night"));
    conditionList.insert(QStringLiteral("10"), QStringLiteral("weather-few-clouds-night"));
    conditionList.insert(QStringLiteral("2"),  QStringLiteral("weather-clouds-night"));
    conditionList.insert(QStringLiteral("20"), QStringLiteral("weather-clouds-night"));
    conditionList.insert(QStringLiteral("80"), QStringLiteral("weather-showers-scattered-night"));
    conditionList.insert(QStringLiteral("95"), QStringLiteral("weather-storm-night"));

    return conditionList;
}

QVector<WeatherData> WetterComParser::parse(const QByteArray &data)
{
    static QMap<QString, QString> dayIcons = setupDayIconMappings();
    QString date, time;
    QVector<WeatherData> wdlist;
    WeatherData weatherData;
    QXmlStreamReader xml(data);
    while (!xml.atEnd()) {
        xml.readNext();

        const QStringRef elementName = xml.name();
        if (xml.isEndElement()) {
            if (elementName == QLatin1String("city")) {
                break;
            }
            if (elementName == QLatin1String("date")) {
                // we have parsed a complete day
                date.clear();
            } else if (elementName == QLatin1String("time")) {
                // we have parsed one forecast

                qDebug() << "Parsed a forecast interval:" << date << time;
                time.clear();
                wdlist.append(weatherData);
                weatherData = WeatherData();
            }
        }
        if (xml.isStartElement()) {
            //qDebug() << "parsing xml elem: " << elementName;

            if (elementName == QLatin1String("date")) {
                date = xml.attributes().value(QStringLiteral("value")).toString();
            } else if (elementName == QLatin1String("time")) {
                time = xml.attributes().value(QStringLiteral("value")).toString();
            } else if (elementName == QLatin1String("tx")) {
                const double tempMax = qRound(xml.readElementText().toDouble());
                qDebug() << "parsed t_max:" << tempMax;
                weatherData.setTemperature(tempMax);
            } else if (elementName == QLatin1String("tn")) {
                const double tempMin = qRound(xml.readElementText().toDouble());
                qDebug() << "parsed t_min:" << tempMin;
                // TODO how to use it?
            } else if (elementName == QLatin1String("w")) {
                int tmp = xml.readElementText().toInt();
                int weather;
//                if (!time.isEmpty())
                    weather = tmp;
//                else
//                    summaryWeather = tmp;

                QString weatherString = QString::number(weather);

                weatherData.setWeatherIcon(dayIcons.value(weatherString));

                qDebug() << "parsed weather condition:" << weatherString;
#if 0
            } else if (elementName == QLatin1String("name")) {
                const QString stationName = xml.readElementText();
                qDebug() << "parsed station name:" << stationName; // VEDENE
            } else if (elementName == QLatin1String("pc")) {
                int tmp = xml.readElementText().toInt();

                if (!time.isEmpty())
                    probability = tmp;
                else
                    summaryProbability = tmp;

                qDebug() << "parsed probability:" << probability;
            } else if (elementName == QLatin1String("text")) {
                const QString credits = xml.readElementText();
                //qDebug() << "parsed credits:" << credits; // "Powered by wetter.com"
            } else if (elementName == QLatin1String("link")) {
                const QString creditsUrl = xml.readElementText();
                //qDebug() << "parsed credits url:" << creditsUrl; // http://www.wetter.com
#endif
            } else if (elementName == QLatin1String("d")) {
                const quint64 localTime = xml.readElementText().toULongLong();
                weatherData.setDateTime(QDateTime::fromTime_t(localTime));
                qDebug() << "parsed local time:" << localTime;
            } else if (elementName == QLatin1String("du")) {
#if 0
                int tmp = xml.readElementText().toInt();
                if (!time.isEmpty())
                    utcTime = tmp;
                else
                    summaryUtcTime = tmp;
                qDebug() << "parsed UTC time:" << tmp;
#endif
            } else {
                //xml.skipCurrentElement();
            }
        }
    }
    return wdlist;
}
