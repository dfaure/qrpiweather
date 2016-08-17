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
    void mergeWithExistingData();

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
    const QVector<WeatherDataEntry> wdlist = parser.parse(data);
    QCOMPARE(wdlist.count(), 40);
    QCOMPARE(wdlist.first().toString(), expectedFirst);
    QCOMPARE(wdlist.last().toString(), expectedLast);

    WeatherData wdData;
    wdData.merge(wdlist);
    QCOMPARE(wdData.count(), 40);
    QCOMPARE(wdData.at(0).toString(), expectedFirst);
    QCOMPARE(wdData.at(39).toString(), expectedLast);
}

void OpenWeatherMapParserTest::mergeWithExistingData()
{
    QByteArray older;
    fetchData(&older, "openweathermap.json");
    QByteArray newer;
    fetchData(&newer, "openweathermap-2.json");

    OpenWeatherMapParser parser;
    QVector<WeatherDataEntry> olderVector = parser.parse(older);
    const QVector<WeatherDataEntry> newerVector = parser.parse(newer);
    WeatherData data;
    data.merge(olderVector);
    QCOMPARE(data.count(), 40);

    data.merge(newerVector);
    QCOMPARE(data.count(), 64);
    QCOMPARE(data.at(0).toString(), QStringLiteral("2016-08-11 21:00:00.000 CEST, temp=18, wind=(25, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png"));
    QCOMPARE(data.at(data.count() - 1).toString(), QStringLiteral("2016-08-19 18:00:00.000 CEST, temp=22, wind=(28, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png"));
    // Check the time intervals are equal (none skipped, none duplicated)
    for (int i = 1; i < data.count(); ++i) {
        const int deltaSecs = data.at(i - 1).dateTime().secsTo(data.at(i).dateTime());
        QCOMPARE(deltaSecs, 3 * 3600); // 3 hours
    }
}

QTEST_MAIN(OpenWeatherMapParserTest)

#include "openweathermapparsertest.moc"
