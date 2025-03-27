#ifndef PERIOD_TIMER_HPP
#define PERIOD_TIMER_HPP

#include <vector>
#include <mutex>
#include <chrono>

#define MAX_EVENT_TIMESTAMPS (1024 * 4)

enum class PeriodEvent {
    AudioBufferFill,
    AccelerometerSample,
    NumEvents
};

struct PeriodStatistics {
    int numSamples = 0;
    double minPeriodInMs = 0.0;
    double maxPeriodInMs = 0.0;
    double avgPeriodInMs = 0.0;
};

class PeriodTimer {
public:
    PeriodTimer();
    ~PeriodTimer();

    void markEvent(PeriodEvent event);
    void getStatisticsAndClear(PeriodEvent event, PeriodStatistics& stats);

    static unsigned long long getCurrentTimeMs();

private:
    struct TimestampData {
        long timestampCount = 0;
        std::vector<long long> timestampsInNs;
        long long prevTimestampInNs = 0;
    };

    void updateStats(const TimestampData& data, PeriodStatistics& stats);
    long long getTimeInNanoS();

    std::mutex mutex_;
    std::vector<TimestampData> eventData_;
    bool initialized;
};

#endif // PERIOD_TIMER_HPP