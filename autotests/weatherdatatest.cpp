#include <QTest>
#include "weatherdata.h"

class WeatherDataTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBasicEntryApi();
    void testBasicDataApi();
};

void WeatherDataTest::testBasicEntryApi()
{
    WeatherDataEntry wd;
    QDateTime dateTime(QDate(2016, 8, 30), QTime(1, 2, 3));
    int celsius = 42;
    double average_wind = 4.2;
    double gust_wind = 5.3;
    int wind_direction = 357;
    double mm_rain = 2.1;
    wd.setTemperatureWindRain(dateTime, celsius, average_wind, gust_wind, wind_direction, mm_rain);
}

void WeatherDataTest::testBasicDataApi()
{
    WeatherData data;
    QCOMPARE(data.count(), 0);
}

QTEST_MAIN(WeatherDataTest)
#include "weatherdatatest.moc"
