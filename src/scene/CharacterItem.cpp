#include "codingstyle.h" // include/codingstyle.h
#include "scene/CharacterItem.h"

CharacterItem::CharacterItem(QObject* parent)
    : Item(parent)
    , m_visible(true)
{
}

void CharacterItem::setPortrait(const QString& portrait) {
    m_portrait = portrait;
}

const QString& CharacterItem::getPortrait() const {
    return m_portrait;
}

void CharacterItem::setExpression(const QString& expression) {
    m_expression = expression;
}

const QString& CharacterItem::getExpression() const {
    return m_expression;
}

void CharacterItem::setVisible(bool visible) {
    m_visible = visible;
}

bool CharacterItem::isVisible() const {
    return m_visible;
}

QString CharacterItem::getType() const {
    return "Character";
}
