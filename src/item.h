#ifndef SRC_ITEM_H
#define SRC_ITEM_H
#include "codingstyle.h" // include/codingstyle.h

#include <QObject>
#include <QString>

class Scene;

class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString className READ className CONSTANT)

public:
    explicit Item(const QString &id, const QString &className, Scene *parent);
    ~Item() override = default;

    QString id() const;
    QString className() const;

signals:
    void activated();

private:
    QString m_id;
    QString m_className;
};

#endif // SRC_ITEM_H
