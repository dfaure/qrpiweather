#include <QObject>
#include <QDebug>

#include <QTest>
#include <QSignalSpy>
#include <QTemporaryFile>

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
    void save();

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
                                         << QStringLiteral("2016-08-11 21:00:00.000 CEST, temp=18, wind=(25, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png, description=Clear sky")
                                         << QStringLiteral("2016-08-16 18:00:00.000 CEST, temp=25, wind=(14, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png, description=Clear sky");
    QTest::newRow("openweathermap-2.json") << "openweathermap-2.json"
                                         << QStringLiteral("2016-08-14 21:00:00.000 CEST, temp=13, wind=(5, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png, description=Clear sky")
                                         << QStringLiteral("2016-08-19 18:00:00.000 CEST, temp=22, wind=(28, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png, description=Clear sky");
    QTest::newRow("openweathermap-3.json") << "openweathermap-3.json"
                                         << QStringLiteral("2016-08-24 21:00:00.000 CEST, temp=13, wind=(5, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png, description=Clear sky")
                                         << QStringLiteral("2016-08-29 18:00:00.000 CEST, temp=22, wind=(28, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01d.png, description=Clear sky");
}

void OpenWeatherMapParserTest::parserShouldFetchDatesAndData()
{
    // Given an input file
    QFETCH(QString, fileName);
    QFETCH(QString, expectedFirst);
    QFETCH(QString, expectedLast);

    QByteArray data;
    fetchData(&data, fileName);

    // When parsing it
    OpenWeatherMapParser parser;
    const QVector<WeatherDataEntry> wdlist = parser.parse(data);

    // Then we should get the expected values
    QCOMPARE(wdlist.count(), 40);
    QCOMPARE(wdlist.first().toString(), expectedFirst);
    QCOMPARE(wdlist.last().toString(), expectedLast);

    // And when "merging" that into a data instance
    WeatherData wdData;
    wdData.merge(wdlist);

    // Then we should get the same values again
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

    for (int iteration = 0; iteration < 2; ++iteration) {
        data.merge(newerVector);
        QCOMPARE(data.count(), 64);
        QCOMPARE(data.at(0).toString(), QStringLiteral("2016-08-11 21:00:00.000 CEST, temp=18, wind=(25, 0, 0), rain=0, icon=http://openweathermap.org/img/w/01n.png, description=Clear sky"));
        QCOMPARE(data.at(data.count() - 1).toString(), newerVector.last().toString());
        // Check the time intervals are equal (none skipped, none duplicated)
        for (int i = 1; i < data.count(); ++i) {
            const int deltaSecs = data.at(i - 1).dateTime().secsTo(data.at(i).dateTime());
            QCOMPARE(deltaSecs, 3 * 3600); // 3 hours
        }
    }

    // Check we only keep one week of old data
    QVERIFY(data.at(0).dateTime().daysTo(data.at(data.count() - 1).dateTime()) <= 10);

    QByteArray later;
    fetchData(&later, "openweathermap-3.json");
    const QVector<WeatherDataEntry> laterVector = parser.parse(later);
    data.merge(laterVector);
    QVERIFY(data.at(0).dateTime().daysTo(data.at(data.count() - 1).dateTime()) <= 10);
    QCOMPARE(data.count(), 47);
}

void OpenWeatherMapParserTest::save()
{
    // Given a vector of weather data entries
    QByteArray data;
    fetchData(&data, "openweathermap.json");
    OpenWeatherMapParser parser;
    const QVector<WeatherDataEntry> wdlist = parser.parse(data);

    // .. and a temp file name
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    const QString fileName = tempFile.fileName();
    tempFile.close();

    // When saving to a file, in OWM format
    QVERIFY(OpenWeatherMapParser::save(wdlist, fileName));

    // Then the file should exist and have the right contents
    QVERIFY(QFile::exists(fileName));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto newList = parser.parse(file.readAll());
    QCOMPARE(newList.count(), wdlist.count());
    for (int i = 0; i < newList.count(); ++i) {
        QCOMPARE(newList.at(i).toString(), wdlist.at(i).toString());
    }
}

QTEST_MAIN(OpenWeatherMapParserTest)

#include "openweathermapparsertest.moc"
