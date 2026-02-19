#ifndef INCLUDE_RESOURCES_FORMATSUPPORT_H
#define INCLUDE_RESOURCES_FORMATSUPPORT_H

#include <QImageReader>
#include <QSet>
#include <QString>

inline const QSet<QString>& supportedImageSuffixes() {
    static QSet<QString> suffixes = []() {
        QSet<QString> values;
        const QList<QByteArray> formats = QImageReader::supportedImageFormats();
        for (const QByteArray& format : formats) {
            values.insert(QString::fromLatin1(format).toLower());
        }
        return values;
    }();
    return suffixes;
}

#endif // INCLUDE_RESOURCES_FORMATSUPPORT_H
