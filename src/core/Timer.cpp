#include "core/Timer.h"

Timer::Timer()
    : m_lastFrameTimeNs(0)
    , m_lastFixedUpdateTimeNs(0)
    , m_deltaTime(0.0f)
    , m_fixedUpdateInterval(1.0f / 60.0f)  // Default 60 FPS for fixed updates
    , m_fixedUpdateAccumulator(0.0f)
    , m_frameCount(0)
    , m_fps(0.0f)
    , m_fpsAccumulator(0.0f)
    , m_fpsFrameCount(0)
{
}

Timer& Timer::getInstance() {
    static Timer instance;
    return instance;
}

void Timer::initialize() {
    m_elapsedTimer.start();
    m_lastFrameTimeNs = m_elapsedTimer.nsecsElapsed();
    m_lastFixedUpdateTimeNs = m_lastFrameTimeNs;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_fps = 0.0f;
    m_fpsAccumulator = 0.0f;
    m_fpsFrameCount = 0;
    m_fixedUpdateAccumulator = 0.0f;
}

void Timer::update() {
    qint64 currentTimeNs = m_elapsedTimer.nsecsElapsed();
    
    // Calculate delta time
    m_deltaTime = static_cast<float>(currentTimeNs - m_lastFrameTimeNs) / 1.0e9f;
    m_lastFrameTimeNs = currentTimeNs;
    
    // Update frame count
    m_frameCount++;
    
    // Accumulate for fixed update
    m_fixedUpdateAccumulator += m_deltaTime;
    
    // Calculate FPS (update every second)
    m_fpsAccumulator += m_deltaTime;
    m_fpsFrameCount++;
    
    if (m_fpsAccumulator >= 1.0f) {
        m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsAccumulator;
        m_fpsAccumulator = 0.0f;
        m_fpsFrameCount = 0;
    }
}

float Timer::getDeltaTime() const {
    return m_deltaTime;
}

float Timer::getRuntime() const {
    return static_cast<float>(m_elapsedTimer.nsecsElapsed()) / 1.0e9f;
}

unsigned long long Timer::getFrameCount() const {
    return m_frameCount;
}

float Timer::getFPS() const {
    return m_fps;
}

bool Timer::shouldFixedUpdate() {
    if (m_fixedUpdateAccumulator >= m_fixedUpdateInterval) {
        m_fixedUpdateAccumulator -= m_fixedUpdateInterval;
        m_lastFixedUpdateTimeNs = m_elapsedTimer.nsecsElapsed();
        return true;
    }
    return false;
}

float Timer::getFixedUpdateInterval() const {
    return m_fixedUpdateInterval;
}

void Timer::setFixedUpdateInterval(float interval) {
    m_fixedUpdateInterval = interval;
}

void Timer::reset() {
    initialize();
}
