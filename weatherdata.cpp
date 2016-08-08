#include "weatherdata.h"

#include <QDateTime>

class WeatherDataPrivate : public QSharedData
{
public:
    QDateTime dateTime;
    int celsius;
    double average_wind;
    double gust_wind;
    int wind_direction;
    double mm_rain;
};

WeatherData::WeatherData()
    : d(new WeatherDataPrivate)
{
}

WeatherData::~WeatherData()
{

}

WeatherData::WeatherData(const WeatherData &other)
    : d(other.d)
{
}

WeatherData::WeatherData(WeatherData &&other) noexcept
    : d(other.d)
{
    other.d = Q_NULLPTR;
}

WeatherData &WeatherData::operator=(const WeatherData &other)
{
    // copy+swap idiom, exception safe
    WeatherData copy(other);
    using namespace std;
    swap(copy.d, d);
    return *this;
}

WeatherData &WeatherData::operator=(WeatherData &&other)
{
    d = other.d;
    other.d = Q_NULLPTR;
    return *this;
}

void WeatherData::setTemperatureWindRain(const QDateTime &dateTime, int celsius, double average_wind, double gust_wind, int wind_direction, double mm_rain)
{
    WeatherDataPrivate *dp = d; // do the detach logic only once
    dp->dateTime = dateTime;
    dp->celsius = celsius;
    dp->average_wind = average_wind;
    dp->gust_wind = gust_wind;
    dp->wind_direction = wind_direction;
    dp->mm_rain = mm_rain;
}

QString WeatherData::toString() const
{
    const WeatherDataPrivate *dp = d;
    return dp->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz t"))
            + ", temp=" + QString::number(dp->celsius)
            + ", wind=(" + QString::number(dp->average_wind) + ", " + QString::number(dp->gust_wind)
            + ", " + QString::number(dp->wind_direction) + "), rain=" + QString::number(dp->mm_rain);
}
