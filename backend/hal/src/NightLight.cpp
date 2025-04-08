#include "NightLight.h"
#include <cstdio>
#include "sharedDataLayout.h"
#include "R5_mmap.h"
#include "Colours.h"

NightLight::NightLight() :isRunning(true) {
    lightSensor = std::make_unique<LightSensor>();
    lightThread = std::thread(&NightLight::processLight, this);
    pR5Base = getR5MmapAddr();
}

NightLight::~NightLight() {
    isRunning = false;
    markLEDs(BLACK, pR5Base);
    freeR5MmapAddr(pR5Base);
    std::printf("Night Light module shutdown.\n");
}

void NightLight::processLight() {
    while (isRunning) {
        auto value = lightSensor->readLightLevel();
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Delay to avoid flashing

        if (value <= 400) {
            markLEDs(WHITE_BRIGHT, pR5Base);
        } else if (value <= 600) {
            markLEDs(WHITE_RGB, pR5Base);
        } else {
            markLEDs(BLACK, pR5Base);
        }
    }
}

void NightLight::markLEDs(uint32_t colour, volatile void* pR5Base) {
    MEM_UINT32((uint8_t*)pR5Base + LED_1_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_2_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_3_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_4_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_5_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_6_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_7_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_8_OFFSET) = colour;
}