#include "codingstyle.h" // include/codingstyle.h
#include "core/Timer.h"

Timer::Timer()
    : m_deltaTime(0.0f)
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
    m_runtimeTimer.start();
    m_lastFrameNs = m_runtimeTimer.nsecsElapsed();
    m_lastFixedUpdateNs = m_lastFrameNs;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_fps = 0.0f;
    m_fpsAccumulator = 0.0f;
    m_fpsFrameCount = 0;
    m_fixedUpdateAccumulator = 0.0f;
}

void Timer::update() {
    const qint64 currentNs = m_runtimeTimer.nsecsElapsed();
    const qint64 elapsedNs = currentNs - m_lastFrameNs;
    m_deltaTime = static_cast<float>(elapsedNs / 1000000000.0);
    m_lastFrameNs = currentNs;
    
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
    return static_cast<float>(m_runtimeTimer.nsecsElapsed() / 1000000000.0);
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
        m_lastFixedUpdateNs = m_runtimeTimer.nsecsElapsed();
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
