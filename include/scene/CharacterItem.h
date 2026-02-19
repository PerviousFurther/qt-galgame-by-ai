#ifndef INCLUDE_SCENE_CHARACTERITEM_H
#define INCLUDE_SCENE_CHARACTERITEM_H

#include "scene/Item.h"

class CharacterItem : public Item {
public:
    explicit CharacterItem(QObject* parent = nullptr);

    void setPortrait(const QString& portrait);
    const QString& getPortrait() const;

    void setExpression(const QString& expression);
    const QString& getExpression() const;

    void setVisible(bool visible);
    bool isVisible() const;

    QString getType() const override;

private:
    QString m_portrait;
    QString m_expression;
    bool m_visible;
};

#endif // INCLUDE_SCENE_CHARACTERITEM_H
