#ifndef CODINGSTYLE_H
#define CODINGSTYLE_H

/**
 * @file include/codingstyle.h
 * @brief Global coding style rules for this repository.
 *
 * Path marker: include/codingstyle.h
 *
 * Required rules:
 * - Prefer Qt types over STL when Qt provides an equivalent.
 * - Use QString/QByteArray instead of std::string.
 * - Use QList/QVector instead of std::vector.
 * - Use QHash/QMap instead of std::map.
 * - Use QSharedPointer/QScopedPointer instead of standard smart pointers (std::shared_ptr/std::unique_ptr).
 * - Use QMutex/QReadWriteLock instead of std::mutex where applicable.
 * - Use QThread instead of std::thread where applicable.
 * - Use qDebug/qInfo/qWarning/qCritical instead of iostream logging.
 * - Custom classes intended for QVariant transport should provide Q_DECLARE_METATYPE.
 *
 * Notes:
 * - Prefer QObject signal/slot over callback wrappers.
 * - Standard exceptions are acceptable where needed.
 */

#endif // CODINGSTYLE_H
