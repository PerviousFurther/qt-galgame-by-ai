#include "codingstyle.h" // include/codingstyle.h
#include "scene/Item.h"

Item::Item(QObject* parent)
    : QObject(parent)
    , m_initialized(false) {
}

Item::~Item() {
    cleanup();
}

const QString& Item::getId() const {
    return m_id;
}

void Item::setId(const QString& id) {
    m_id = id;
}

const QString& Item::getName() const {
    return m_name;
}

void Item::setName(const QString& name) {
    m_name = name;
}

void Item::initialize() {
    m_initialized = true;
}

void Item::update() {
    // Base implementation does nothing
    // Derived classes can override to implement specific behavior
    // Use Timer::getInstance().getDeltaTime() to get delta time in seconds
}

void Item::fixedUpdate() {
    // Base implementation does nothing
    // Derived classes can override for fixed-interval updates (physics, mini-games)
    // Use Timer::getInstance().getFixedUpdateInterval() to get the interval
}

void Item::cleanup() {
    m_initialized = false;
}

QString Item::getType() const {
    return "Item";
}

AudioItem::AudioItem(QObject* parent)
    : Item(parent)
    , m_loop(false)
    , m_playing(false)
{
}

void AudioItem::setSource(const QString& source) {
    m_source = source;
}

const QString& AudioItem::getSource() const {
    return m_source;
}

void AudioItem::setLoop(bool loop) {
    m_loop = loop;
}

bool AudioItem::isLoop() const {
    return m_loop;
}

void AudioItem::play() {
    m_playing = true;
    if (m_playHandler) {
        m_playHandler();
    }
}

void AudioItem::stop() {
    m_playing = false;
    if (m_stopHandler) {
        m_stopHandler();
    }
}

bool AudioItem::isPlaying() const {
    return m_playing;
}

void AudioItem::setPlayHandler(const std::function<void()>& handler) {
    m_playHandler = handler;
}

void AudioItem::setStopHandler(const std::function<void()>& handler) {
    m_stopHandler = handler;
}

QString AudioItem::getType() const {
    return "Audio";
}

VideoItem::VideoItem(QObject* parent)
    : Item(parent)
    , m_loop(false)
    , m_playing(false)
{
}

void VideoItem::setSource(const QString& source) {
    m_source = source;
}

const QString& VideoItem::getSource() const {
    return m_source;
}

void VideoItem::setLoop(bool loop) {
    m_loop = loop;
}

bool VideoItem::isLoop() const {
    return m_loop;
}

void VideoItem::play() {
    m_playing = true;
    if (m_playHandler) {
        m_playHandler();
    }
}

void VideoItem::stop() {
    m_playing = false;
    if (m_stopHandler) {
        m_stopHandler();
    }
}

bool VideoItem::isPlaying() const {
    return m_playing;
}

void VideoItem::setPlayHandler(const std::function<void()>& handler) {
    m_playHandler = handler;
}

void VideoItem::setStopHandler(const std::function<void()>& handler) {
    m_stopHandler = handler;
}

QString VideoItem::getType() const {
    return "Video";
}

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
