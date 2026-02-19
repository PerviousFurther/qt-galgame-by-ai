#ifndef EXECUTION_H
#define EXECUTION_H

#include <QElapsedTimer>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <utility>

/**
 * @brief Global execution singleton for timing and task dispatch.
 *
 * Execution upgrades the old Timer role by:
 * - Keeping frame timing/fixed update information
 * - Dispatching asynchronous tasks through an internal thread pool
 * - Dispatching delayed/timed tasks
 */
class Execution {
public:
    static Execution& getInstance();

    void initialize();
    void update();

    float getDeltaTime() const;
    float getRuntime() const;
    unsigned long long getFrameCount() const;
    float getFPS() const;

    bool shouldFixedUpdate();
    float getFixedUpdateInterval() const;
    void setFixedUpdateInterval(float interval);

    void reset();

    int getMaxThreadCount() const;
    void setMaxThreadCount(int threadCount);

    template <typename Callable>
    void dispatchAsyncTask(Callable&& task) {
        m_threadPool.start(QRunnable::create(std::forward<Callable>(task)));
    }

    template <typename Callable>
    void dispatchTimedTask(int delayMs, Callable&& task) {
        QTimer::singleShot(delayMs, [task = std::forward<Callable>(task)]() mutable {
            Execution::getInstance().dispatchAsyncTask(std::move(task));
        });
    }

private:
    Execution();
    ~Execution() = default;
    Execution(const Execution&) = delete;
    Execution& operator=(const Execution&) = delete;

    QElapsedTimer m_runtimeTimer;
    qint64 m_lastFrameNs;
    qint64 m_lastFixedUpdateNs;

    float m_deltaTime;
    float m_fixedUpdateInterval;
    float m_fixedUpdateAccumulator;

    unsigned long long m_frameCount;
    float m_fps;
    float m_fpsAccumulator;
    int m_fpsFrameCount;

    QThreadPool m_threadPool;
};

#endif // EXECUTION_H
