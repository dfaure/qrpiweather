#ifndef WEATHERMODEL_H
#define WEATHERMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include "dataprovider.h"
#include "weatherdata.h"
class WeatherParserBase;
class QIODevice;
class QTimer;

class WeatherModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString backendName READ backendName NOTIFY backendNameChanged)
    Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)
    Q_PROPERTY(int indexForCurrentDateTime READ indexForCurrentDateTime)

public:
    explicit WeatherModel(QObject *parent = 0);
    ~WeatherModel();

    enum Roles {
        DayOfWeek = Qt::UserRole + 90,
        Time,
        DateTime,
        Temperature,
        AverageWind,
        GustWind,
        WindDirection,
        Rain,
        WeatherIcon,
        WeatherDescription
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // QML interface
    Q_INVOKABLE void toggleBackend();
    QString backendName() const;
    QDateTime currentDateTime() const;
    int indexForCurrentDateTime() const;

signals:
    void backendNameChanged();
    void currentDateTimeChanged();

private slots:
    void slotDataAvailable(const QByteArray &data);
    void slotError();
    void fetchData();

private:
    QString autoSaveFileName() const;

    WeatherData m_data;

    enum Backend {
        InfoClimat,
        WetterCom,
        OpenWeatherMap,
        LastBackend = OpenWeatherMap
    };
    void setBackend(Backend backend);
    Backend m_backend;

    WeatherParserBase *m_parser;
    DataProvider::Ptr m_provider;
    QTimer *m_reloadTimer;
};

#endif // WEATHERMODEL_H
