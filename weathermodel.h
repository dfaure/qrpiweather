#ifndef WEATHERMODEL_H
#define WEATHERMODEL_H

#include <QAbstractTableModel>
#include "dataprovider.h"
#include "weatherdata.h"
class WeatherParserBase;
class QIODevice;

class WeatherModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString backendName READ backendName NOTIFY backendNameChanged)

public:
    explicit WeatherModel(QObject *parent = 0);
    ~WeatherModel();

    enum Roles {
        DayOfWeek = Qt::UserRole + 90,
        Time,
        Temperature,
        AverageWind,
        GustWind,
        WindDirection,
        Rain,
        WeatherIcon
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void toggleBackend();
    QString backendName() const;

signals:
    void backendNameChanged();

private slots:
    void slotDataAvailable(const QByteArray &data);
    void slotError();

private:
    void fetchData();

    QVector<WeatherData> m_data;

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
};

#endif // WEATHERMODEL_H
