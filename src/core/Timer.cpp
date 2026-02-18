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
    m_startTime = Clock::now();
    m_lastFrameTime = m_startTime;
    m_lastFixedUpdateTime = m_startTime;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_fps = 0.0f;
    m_fpsAccumulator = 0.0f;
    m_fpsFrameCount = 0;
    m_fixedUpdateAccumulator = 0.0f;
}

void Timer::update() {
    TimePoint currentTime = Clock::now();
    
    // Calculate delta time
    std::chrono::duration<float> elapsed = currentTime - m_lastFrameTime;
    m_deltaTime = elapsed.count();
    m_lastFrameTime = currentTime;
    
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
    auto currentTime = Clock::now();
    std::chrono::duration<float> elapsed = currentTime - m_startTime;
    return elapsed.count();
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
        m_lastFixedUpdateTime = Clock::now();
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
