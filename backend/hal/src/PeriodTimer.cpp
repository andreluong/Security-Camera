#include "PeriodTimer.h"
#include <iostream>
#include <cstring>
#include <cassert>
#include <ctime>

#define NS_PER_S 1000000000LL
#define MS_PER_NS 1000000.0

PeriodTimer::PeriodTimer()
    : initialized(true), eventData_(static_cast<size_t>(PeriodEvent::NumEvents)) {
    for (auto& data : eventData_) {
        data.timestampsInNs.reserve(MAX_EVENT_TIMESTAMPS);
    }
}

PeriodTimer::~PeriodTimer() {
    initialized = false;
}

void PeriodTimer::markEvent(PeriodEvent event) {
    assert(initialized);
    size_t index = static_cast<size_t>(event);
    assert(index < eventData_.size());

    std::lock_guard<std::mutex> lock(mutex_);
    TimestampData& data = eventData_[index];

    if (data.timestampCount < MAX_EVENT_TIMESTAMPS) {
        data.timestampsInNs.push_back(getTimeInNanoS());
        data.timestampCount++;
    } else {
        std::cerr << "WARNING: No sample space for event collection on " << index << "\n";
    }
}

void PeriodTimer::getStatisticsAndClear(PeriodEvent event, PeriodStatistics& stats) {
    assert(initialized);
    size_t index = static_cast<size_t>(event);
    assert(index < eventData_.size());

    std::lock_guard<std::mutex> lock(mutex_);
    TimestampData& data = eventData_[index];

    updateStats(data, stats);

    if (!data.timestampsInNs.empty()) {
        data.prevTimestampInNs = data.timestampsInNs.back();
    }

    data.timestampCount = 0;
    data.timestampsInNs.clear();
}

void PeriodTimer::updateStats(const TimestampData& data, PeriodStatistics& stats) {
    long long prevNs = data.prevTimestampInNs;

    if (prevNs == 0 && !data.timestampsInNs.empty()) {
        prevNs = data.timestampsInNs[0];
    }

    long long sumDeltasNs = 0;
    long long minNs = 0;
    long long maxNs = 0;

    for (size_t i = 0; i < data.timestampsInNs.size(); ++i) {
        long long current = data.timestampsInNs[i];
        long long delta = current - prevNs;
        sumDeltasNs += delta;

        if (i == 0 || delta < minNs) minNs = delta;
        if (i == 0 || delta > maxNs) maxNs = delta;

        prevNs = current;
    }

    long long avgNs = 0;
    if (!data.timestampsInNs.empty()) {
        avgNs = sumDeltasNs / data.timestampsInNs.size();
    }

    stats.numSamples = data.timestampsInNs.size();
    stats.minPeriodInMs = minNs / MS_PER_NS;
    stats.maxPeriodInMs = maxNs / MS_PER_NS;
    stats.avgPeriodInMs = avgNs / MS_PER_NS;
}

long long PeriodTimer::getTimeInNanoS() {
    struct timespec spec;
    clock_gettime(CLOCK_BOOTTIME, &spec);
    return static_cast<long long>(spec.tv_sec) * NS_PER_S + spec.tv_nsec;
}

unsigned long long PeriodTimer::getCurrentTimeMs() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<unsigned long long>(now.tv_sec) * 1000 + now.tv_nsec / 1000000;
}