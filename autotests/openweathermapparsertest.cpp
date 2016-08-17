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
    void parserShouldFetchDatesAndData_data();
    void parserShouldFetchDatesAndData();

private:
    void fetchData(QByteArray *data, const QString &fileName);
};

void OpenWeatherMapParserTest::fetchData(QByteArray *data, const QString &fileName)
{
    DataProvider::Ptr provider(new FileDataProvider(QFINDTESTDATA(fileName)));
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
    fetchData(&data, "openweathermap.json");
}

void OpenWeatherMapParserTest::parserShouldFetchDatesAndData_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("expectedFirst");
    QTest::addColumn<QString>("expectedLast");

    QTest::newRow("openweathermap.json") << "openweathermap.json"
                                         << QStringLiteral("2016-08-11 21:00:00.000 CEST, temp=18, wind=(25, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png")
                                         << QStringLiteral("2016-08-16 18:00:00.000 CEST, temp=25, wind=(14, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png");
    QTest::newRow("openweathermap-2.json") << "openweathermap-2.json"
                                         << QStringLiteral("2016-08-14 21:00:00.000 CEST, temp=13, wind=(5, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png")
                                         << QStringLiteral("2016-08-19 18:00:00.000 CEST, temp=22, wind=(28, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png");
}

void OpenWeatherMapParserTest::parserShouldFetchDatesAndData()
{
    QFETCH(QString, fileName);
    QFETCH(QString, expectedFirst);
    QFETCH(QString, expectedLast);

    QByteArray data;
    fetchData(&data, fileName);

    OpenWeatherMapParser parser;
    QVector<WeatherDataEntry> wdlist = parser.parse(data);
    QCOMPARE(wdlist.count(), 40);
    QCOMPARE(wdlist.first().toString(), expectedFirst);
    QCOMPARE(wdlist.last().toString(), expectedLast);
}

QTEST_MAIN(OpenWeatherMapParserTest)

#include "openweathermapparsertest.moc"
