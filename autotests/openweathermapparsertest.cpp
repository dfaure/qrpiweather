#include <QObject>
#include <QDebug>

#include <QTest>
#include <QSignalSpy>

#include "filedataprovider.h"
#include "openweathermapparser.h"

class OpenWeatherMapParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fileDataProviderShouldProvideData();
    void parserShouldFetchDatesAndData();

private:
    void fetchData(QByteArray *data);
};

void OpenWeatherMapParserTest::fetchData(QByteArray *data)
{
    DataProvider::Ptr provider(new FileDataProvider(QFINDTESTDATA("openweathermap.json")));
    provider->ensureDataAvailable();
    QSignalSpy spyError(provider.data(), &DataProvider::error);
    QSignalSpy spyDataAvailable(provider.data(), &DataProvider::dataAvailable);

    QVERIFY(spyDataAvailable.wait(1000));
    QVERIFY(spyError.isEmpty());
    *data = spyDataAvailable.at(0).at(0).value<QByteArray>();
}

void OpenWeatherMapParserTest::fileDataProviderShouldProvideData()
{
    QByteArray data;
    fetchData(&data);
}

void OpenWeatherMapParserTest::parserShouldFetchDatesAndData()
{
    QByteArray data;
    fetchData(&data);

    OpenWeatherMapParser parser;
    QVector<WeatherDataEntry> wdlist = parser.parse(data);
    QCOMPARE(wdlist.count(), 40);
    QCOMPARE(wdlist.first().toString(), QStringLiteral("2016-08-11 21:00:00.000 CEST, temp=18, wind=(25, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png"));
    QCOMPARE(wdlist.last().toString(), QStringLiteral("2016-08-16 18:00:00.000 CEST, temp=25, wind=(14, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png"));
}

QTEST_MAIN(OpenWeatherMapParserTest)

#include "openweathermapparsertest.moc"
