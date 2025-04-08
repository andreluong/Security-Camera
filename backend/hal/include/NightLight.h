#pragma once

#include "LightSensor.h"
#include <memory>
#include <thread>
#include <atomic>
#include <cstdint>

class NightLight {
public:
    NightLight();
    ~NightLight();

private:
    std::atomic<bool> isRunning;
    std::unique_ptr<LightSensor> lightSensor;
    std::thread lightThread;
    volatile void* pR5Base;

    void processLight();
    void markLEDs(uint32_t colour, volatile void* pR5Base);
};