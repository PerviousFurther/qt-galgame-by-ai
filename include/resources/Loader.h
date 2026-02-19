#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QString>
#include <QVariant>

class Loader : public QObject {
    Q_OBJECT
public:
    explicit Loader(const QString& protocol, const QString& suffix, QObject* parent = nullptr);
    ~Loader() override = default;

    const QString& getProtocol() const;
    const QString& getSuffix() const;

    virtual QVariant load(const QVariant& source) = 0;

private:
    QString m_protocol;
    QString m_suffix;
};

class BitmapLoader : public Loader {
    Q_OBJECT
public:
    explicit BitmapLoader(QObject* parent = nullptr);
    QVariant load(const QVariant& source) override;
};

class VideoLoader : public Loader {
    Q_OBJECT
public:
    explicit VideoLoader(QObject* parent = nullptr);
    QVariant load(const QVariant& source) override;
};

#endif // LOADER_H
