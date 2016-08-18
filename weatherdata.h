#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QSharedDataPointer>
#include <QMetaType>
#include <QVector>

class WeatherDataEntryPrivate;

class WeatherDataEntry
{
public:
    WeatherDataEntry();
    ~WeatherDataEntry();
    WeatherDataEntry(const WeatherDataEntry &other);
    WeatherDataEntry(WeatherDataEntry &&other) noexcept;

    WeatherDataEntry &operator=(const WeatherDataEntry &other);
    WeatherDataEntry &operator=(WeatherDataEntry &&other);

    void setTemperatureWindRain(const QDateTime &dateTime,
                                int temperature_celsius,
                                double average_wind, double gust_wind, int wind_direction,
                                double mm_rain);
    void setDateTime(const QDateTime &dateTime);
    void setTemperature(int temperature_celsius);
    void setWeatherIcon(const QString &icon, const QString &description);

    QString toString() const;

    QDateTime dateTime() const;
    int temperature_celsius() const;
    int average_wind() const;
    int gust_wind() const;
    int wind_direction() const;
    double mm_rain() const;
    QString weather_icon() const;
    QString weather_description() const;

private:
    QSharedDataPointer<WeatherDataEntryPrivate> d;
};

class WeatherData
{
public:
    int count() const { return m_data.count(); }
    WeatherDataEntry at(int i) const { return m_data.at(i); }

    void merge(const QVector<WeatherDataEntry> &vec);

private:
    QVector<WeatherDataEntry> m_data;
};


Q_DECLARE_METATYPE(WeatherDataEntry)
Q_DECLARE_TYPEINFO(WeatherDataEntry, Q_MOVABLE_TYPE);

#endif // WEATHERDATA_H
