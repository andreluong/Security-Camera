#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "personDetector.h"
#include "i2cHelpers.h"
#include "lightSensor.h"
#include "cameraFeed.h"
#include "CommandServer.h"
#include "Microservo.h"
#include "PanTiltKit.h"
#include <memory>
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
#include <signal.h>
#include "light_sensor.h"
#include "sharedDataLayout.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

#define NUM_LEDS 8
#define SHARED_MEM_LENGTH 0x8000
#define SHARED_MEM_BASE 0x79020000

#define WHITE_BRIGHT  0xffffff00
#define WHITE_DIM     0x0f0f0f00

volatile void* pSharedMem = NULL;
static bool keepRunning = 0;

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

volatile void* map_shared_memory(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem");
        exit(EXIT_FAILURE);
    }

    volatile void* addr = mmap(0, SHARED_MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SHARED_MEM_BASE);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return addr;
}

void freeR5MmapAddr(volatile void* addr)
{
    if (munmap((void*) addr, SHARED_MEM_LENGTH)) {
        perror("R5 munmap failed");
        exit(EXIT_FAILURE);
    }
}

void handleSigint(int sig) {
    (void)sig;
    printf("\nShutting down...\n");
    keepRunning = 0;
}

void write_led_color(int index, uint32_t color) {
    int offset = SHARED_RGB_OFFSET + (index * 4);
    volatile uint32_t* pAddr = (volatile uint32_t *)((volatile uint8_t *)pSharedMem + offset);
    *pAddr = color;
}

void test(){
    signal(SIGINT, handleSigint);
    LightSensor sensor;

    pSharedMem = map_shared_memory();
    *((volatile uint32_t *)((uint8_t *)pSharedMem + LED_DELAY_MS_OFFSET)) = 10;

    while (keepRunning) {
        uint16_t lightLevel = sensor.readLightLevel();
        printf("Light Level: 0x%03X = %d\n", lightLevel, lightLevel);
        sleep(1);

        if(lightLevel <= 400){
            for (int i = 0; i < NUM_LEDS; i++) {
                write_led_color(i, WHITE_BRIGHT);
            }
        }else if (lightLevel > 400 && lightLevel <= 600){
            for (int i = 0; i < NUM_LEDS; i++) {
                write_led_color(i, WHITE_DIM);
            }
        }else{
            for (int i = 0; i < NUM_LEDS; i++) {
                write_led_color(i, 0x00000000);
            }
        }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        write_led_color(i, 0x00000000); // Off
    }
    
    freeR5MmapAddr(pSharedMem);
}

int main() {
    std::cout << "Starting server\n";

    test();
    return 0;

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector);
     
    auto pan = std::make_unique<Microservo>("/dev/hat/pwm/GPIO6/", 10, 0, 180);
    auto tilt = std::make_unique<Microservo>("/dev/hat/pwm/GPIO14/", 30, 0, 90);
    PanTiltKit panTiltKit(std::move(pan), std::move(tilt));

    CommandServer commandServer = CommandServer(panTiltKit, personDetector);

    std::thread broadcastThread([&]() {
        broadcastServer.run(9002);
    });

    std::thread cameraFeedThread([&]() {
        cameraFeed.captureAndQueueFrame();
    });

    std::thread cameraSendThread([&]() {
        cameraFeed.dequeAndSendFrame(broadcastServer);
    });

    std::thread commandThread([&]() {
        commandServer.run(9001);
    });

    commandThread.join();
    cameraSendThread.join();
    cameraFeedThread.join();   
    broadcastThread.join();

    std::cout << "Closing server\n";

    return 0;
}
