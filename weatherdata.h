#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QSharedDataPointer>
#include <QMetaType>

class WeatherDataPrivate;

class WeatherData
{
public:
    WeatherData();
    ~WeatherData();
    WeatherData(const WeatherData &other);
    WeatherData(WeatherData &&other) noexcept;

    WeatherData &operator=(const WeatherData &other);
    WeatherData &operator=(WeatherData &&other);

    void setTemperatureWindRain(const QDateTime &dateTime,
                                int celsius,
                                double average_wind, double gust_wind, int wind_direction,
                                double mm_rain);
    QString toString() const;

private:
    QSharedDataPointer<WeatherDataPrivate> d;
};

Q_DECLARE_METATYPE(WeatherData)
Q_DECLARE_TYPEINFO(WeatherData, Q_MOVABLE_TYPE);

#endif // WEATHERDATA_H
