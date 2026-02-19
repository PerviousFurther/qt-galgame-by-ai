#ifndef ITEM_H
#define ITEM_H
#include "codingstyle.h" // include/codingstyle.h

#include <QObject>
#include <QString>
#include <QSharedPointer>

/**
 * @brief Base class for all items that can be placed in a scene.
 * 
 * Item is the fundamental building block of the galgame engine.
 * Most objects in the scene inherit from this class.
 */
class Item : public QObject {
    Q_OBJECT
public:
    explicit Item(QObject* parent = nullptr);
    virtual ~Item();

    /**
     * @brief Get the unique identifier of this item
     * @return The item's ID
     */
    const QString& getId() const;

    /**
     * @brief Set the unique identifier of this item
     * @param id The new ID
     */
    void setId(const QString& id);

    /**
     * @brief Get the name of this item
     * @return The item's name
     */
    const QString& getName() const;

    /**
     * @brief Set the name of this item
     * @param name The new name
     */
    void setName(const QString& name);

    /**
     * @brief Initialize the item
     * Called when the item is added to a scene
     */
    virtual void initialize();

    /**
     * @brief Update the item state
     * Called every frame. Use Timer::getInstance().getDeltaTime() to get delta time.
     */
    virtual void update();

    /**
     * @brief Fixed update for physics and time-critical operations
     * Called at fixed intervals (e.g., for mini-games like Snake).
     * Use Timer::getInstance().getFixedUpdateInterval() to get the interval.
     */
    virtual void fixedUpdate();

    /**
     * @brief Clean up resources when item is removed
     */
    virtual void cleanup();

    /**
     * @brief Get the runtime type name of this Item.
     */
    virtual QString getType() const;

protected:
    QString m_id;
    QString m_name;
    bool m_initialized;
};

class AudioItem : public Item {
    Q_OBJECT
public:
    explicit AudioItem(QObject* parent = nullptr);

    void setSource(const QString& source);
    const QString& getSource() const;

    void setLoop(bool loop);
    bool isLoop() const;

    void play();
    void stop();
    bool isPlaying() const;

    QString getType() const override;

signals:
    void playRequested();
    void stopRequested();

private:
    QString m_source;
    bool m_loop;
    bool m_playing;
};

class VideoItem : public Item {
    Q_OBJECT
public:
    explicit VideoItem(QObject* parent = nullptr);

    void setSource(const QString& source);
    const QString& getSource() const;

    void setLoop(bool loop);
    bool isLoop() const;

    void play();
    void stop();
    bool isPlaying() const;

    QString getType() const override;

signals:
    void playRequested();
    void stopRequested();

private:
    QString m_source;
    bool m_loop;
    bool m_playing;
};

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

#endif // ITEM_H
