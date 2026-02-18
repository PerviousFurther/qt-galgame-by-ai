#include "item.h"
#include "scene.h"

Item::Item(const QString &id, const QString &className, Scene *parent)
    : QObject(parent)
    , m_id(id)
    , m_className(className)
{
}

QString Item::id() const
{
    return m_id;
}

QString Item::className() const
{
    return m_className;
}
