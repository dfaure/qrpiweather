#include <QObject>
#include <QDebug>

#include <QTest>
#include <QSignalSpy>

#include "filedataprovider.h"
#include "wettercomparser.h"

class WetterComParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fileDataProviderShouldProvideData();
    void parserShouldFetchDatesAndData();

private:
    void fetchData(QByteArray *data);
};

void WetterComParserTest::fetchData(QByteArray *data)
{
    DataProvider::Ptr provider(new FileDataProvider(QFINDTESTDATA("wettercom.xml")));
    provider->ensureDataAvailable();
    QSignalSpy spyError(provider.data(), &DataProvider::error);
    QSignalSpy spyDataAvailable(provider.data(), &DataProvider::dataAvailable);

    QVERIFY(spyDataAvailable.wait(1000));
    QVERIFY(spyError.isEmpty());
    *data = spyDataAvailable.at(0).at(0).value<QByteArray>();
}

void WetterComParserTest::fileDataProviderShouldProvideData()
{
    QByteArray data;
    fetchData(&data);
}

void WetterComParserTest::parserShouldFetchDatesAndData()
{
    QByteArray data;
    fetchData(&data);

    QString output;
    QTextStream stream(&output);
    WetterComParser parser(stream);
    QVector<WeatherData> wdlist = parser.parse(data);
    QCOMPARE(wdlist.count(), 12);
    QCOMPARE(wdlist.last().toString(), QStringLiteral("2016-08-14 01:00:00.000 CEST, temp=26, wind=(0, 0, 0), rain=0, icon=weather-few-clouds"));
    QVERIFY2(output.isEmpty(), qPrintable(output));
}

QTEST_MAIN(WetterComParserTest)

#include "wettercomparsertest.moc"
