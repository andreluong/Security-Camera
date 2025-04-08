#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "personDetector.h"
#include "i2cHelpers.h"
// #include "lightSensor.h"
#include "cameraFeed.h"
#include "CommandServer.h"
#include "Microservo.h"
#include "PanTiltKit.h"
#include "PlaySound.h"
#include <memory>
#include "joystick.h"
#include "rotary_button.h"
#include "gpio.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

// -----
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "lightSensor2.h"
#include "sharedDataLayout.h"
#include "R5_mmap.h"

constexpr int NUM_LEDS = 8;
constexpr int SHARED_MEM_LENGTH = 0x8000;
constexpr unsigned int SHARED_MEM_BASE = 0x79020000;

constexpr unsigned int WHITE_BRIGHT = 0xffffff00;
constexpr unsigned int WHITE_DIM    = 0x0f0f0f00;
#define TEAL_LED  0x0f000f00 // Teal

void markLEDs(uint32_t colour, volatile void* pR5Base) {
    MEM_UINT32((uint8_t*)pR5Base + LED_1_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_2_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_3_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_4_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_5_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_6_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_7_OFFSET) = colour;
    MEM_UINT32((uint8_t*)pR5Base + LED_8_OFFSET) = colour;
}

void test(){
    // Get access to shared memory for my uses
    auto pR5Base = getR5MmapAddr();

    LightSensor sensor;

    while (true) {
        auto lightLevel = sensor.readLightLevel();
        printf("Light Level: 0x%03X = %d\n", lightLevel, lightLevel);

        // if (lightLevel <= 400) {
        //     printf("bright light\n");
        //     markLEDs(TEAL_LED, pR5Base);
        // } else if (lightLevel <= 600){
        //     markLEDs(WHITE_DIM, pR5Base);
        // } else {
        //     markLEDs(TEAL_LED, pR5Base);
        // }
        markLEDs(TEAL_LED, pR5Base);
        sleep(1);
    }
    markLEDs(0x00000000, pR5Base);
    freeR5MmapAddr(pR5Base);
}

// ------

int main() {
    std::cout << "Starting server\n";
    Gpio gpio;

    auto pR5Base = getR5MmapAddr();

    LightSensor sensor;

    while (true) {
        auto lightLevel = sensor.readLightLevel();
        printf("Light Level: 0x%03X = %d\n", lightLevel, lightLevel);
        markLEDs(TEAL_LED, pR5Base);
        sleep(1);
    }
    markLEDs(0x00000000, pR5Base);
    freeR5MmapAddr(pR5Base);

    // test();
    return 0;

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector, broadcastServer);

    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit);
    RotaryButton button;

    CommandServer commandServer = CommandServer(panTiltKit, personDetector);

    while(!button.isPressed()) {

    }
    //Just testing sound
    // Audio::init();
    // Sound sound;
    // sound.playSound();

    //commandThread.join();
    // cameraSendThread.join();
    // cameraFeedThread.join();   
    //broadcastThread.join();

    std::cout << "Closing server\n";

    return 0;
}
