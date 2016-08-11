#ifndef THEMEIMAGEPROVIDER_H
#define THEMEIMAGEPROVIDER_H

#include <QQuickImageProvider>

class ThemeImageProvider : public QQuickImageProvider
{
public:
    ThemeImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) Q_DECL_OVERRIDE;
};

#endif // THEMEIMAGEPROVIDER_H
