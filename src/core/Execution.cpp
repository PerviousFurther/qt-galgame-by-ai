#include "core/Execution.h"

#include "core/Configuration.h"

namespace {
constexpr double NanosecondsToSeconds = 1e-9;
}

Execution::Execution()
    : m_lastFrameNs(0)
    , m_lastFixedUpdateNs(0)
    , m_deltaTime(0.0f)
    , m_fixedUpdateInterval(1.0f / 60.0f)
    , m_fixedUpdateAccumulator(0.0f)
    , m_frameCount(0)
    , m_fps(0.0f)
    , m_fpsAccumulator(0.0f)
    , m_fpsFrameCount(0)
{
}

Execution& Execution::getInstance() {
    static Execution instance;
    return instance;
}

void Execution::initialize() {
    m_runtimeTimer.start();
    m_lastFrameNs = m_runtimeTimer.nsecsElapsed();
    m_lastFixedUpdateNs = m_lastFrameNs;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_fps = 0.0f;
    m_fpsAccumulator = 0.0f;
    m_fpsFrameCount = 0;
    m_fixedUpdateAccumulator = 0.0f;

    const int configuredMaxThreads = Configuration::getInstance()
        .getValue(QStringLiteral("execution.max_threads"), QThread::idealThreadCount())
        .toInt();
    setMaxThreadCount(configuredMaxThreads);
}

void Execution::update() {
    const qint64 currentNs = m_runtimeTimer.nsecsElapsed();
    const qint64 elapsedNs = currentNs - m_lastFrameNs;
    m_deltaTime = static_cast<float>(elapsedNs * NanosecondsToSeconds);
    m_lastFrameNs = currentNs;

    m_frameCount++;
    m_fixedUpdateAccumulator += m_deltaTime;

    m_fpsAccumulator += m_deltaTime;
    m_fpsFrameCount++;

    if (m_fpsAccumulator >= 1.0f) {
        m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsAccumulator;
        m_fpsAccumulator = 0.0f;
        m_fpsFrameCount = 0;
    }
}

float Execution::getDeltaTime() const {
    return m_deltaTime;
}

float Execution::getRuntime() const {
    return static_cast<float>(m_runtimeTimer.nsecsElapsed() * NanosecondsToSeconds);
}

unsigned long long Execution::getFrameCount() const {
    return m_frameCount;
}

float Execution::getFPS() const {
    return m_fps;
}

bool Execution::shouldFixedUpdate() {
    if (m_fixedUpdateAccumulator >= m_fixedUpdateInterval) {
        m_fixedUpdateAccumulator -= m_fixedUpdateInterval;
        m_lastFixedUpdateNs = m_runtimeTimer.nsecsElapsed();
        return true;
    }
    return false;
}

float Execution::getFixedUpdateInterval() const {
    return m_fixedUpdateInterval;
}

void Execution::setFixedUpdateInterval(float interval) {
    m_fixedUpdateInterval = interval;
}

void Execution::reset() {
    initialize();
}

int Execution::getMaxThreadCount() const {
    return m_threadPool.maxThreadCount();
}

void Execution::setMaxThreadCount(int threadCount) {
    if (threadCount <= 0) {
        threadCount = 1;
    }
    m_threadPool.setMaxThreadCount(threadCount);
}
