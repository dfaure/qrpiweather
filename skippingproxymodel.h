#ifndef SKIPPINGPROXYMODEL_H
#define SKIPPINGPROXYMODEL_H

#include <QSortFilterProxyModel>

class SkippingProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int indexForCurrentDateTime READ indexForCurrentDateTime)

public:
    SkippingProxyModel(QObject *parent = Q_NULLPTR);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

    // QML interface
    int indexForCurrentDateTime() const;
};

#endif // SKIPPINGPROXYMODEL_H
