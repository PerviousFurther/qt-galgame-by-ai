#ifndef TIMER_H
#define TIMER_H
#include "codingstyle.h" // include/codingstyle.h

#include <QElapsedTimer>

/**
 * @brief Global timer singleton for managing frame timing and updates
 * 
 * Timer provides timing information for the game engine, including:
 * - Total runtime since application start
 * - Delta time between frames
 * - Fixed update timing for physics/mini-games
 * 
 * This singleton should be accessed throughout the engine to maintain
 * consistent timing across all systems.
 */
class Timer {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Timer singleton
     */
    static Timer& getInstance();

    /**
     * @brief Initialize the timer
     * Should be called once at application start
     */
    void initialize();

    /**
     * @brief Update the timer state
     * Should be called once per frame
     */
    void update();

    /**
     * @brief Get the time elapsed since last frame in seconds
     * @return Delta time in seconds
     */
    float getDeltaTime() const;

    /**
     * @brief Get the total runtime since initialization in seconds
     * @return Runtime in seconds
     */
    float getRuntime() const;

    /**
     * @brief Get the current frame count
     * @return Total number of frames since initialization
     */
    unsigned long long getFrameCount() const;

    /**
     * @brief Get the current frames per second
     * @return Current FPS
     */
    float getFPS() const;

    /**
     * @brief Check if a fixed update should occur
     * @return true if enough time has elapsed for a fixed update
     */
    bool shouldFixedUpdate();

    /**
     * @brief Get the fixed update interval in seconds
     * @return Fixed update interval
     */
    float getFixedUpdateInterval() const;

    /**
     * @brief Set the fixed update interval in seconds
     * @param interval Fixed update interval (default: 1/60 = ~0.0166s)
     */
    void setFixedUpdateInterval(float interval);

    /**
     * @brief Reset the timer
     */
    void reset();

private:
    Timer();
    ~Timer() = default;
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

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
};

#endif // TIMER_H
