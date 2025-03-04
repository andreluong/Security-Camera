#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "i2cHelpers.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

// Captures current frame and sends to all connections
void captureAndSend(BroadcastServer& broadcastServer) {
    cv::VideoCapture capture(USB_CAMERA_PORT);
    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera\n";
        return;
    }

    cv::Mat frame;
    while (true) {
        capture >> frame;
        if (frame.empty()) break;
        
        broadcastServer.sendFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_DELAY_MS));
    }
}

int main() {
    BroadcastServer broadcastServer;
    int i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    close(i2c_file_desc);

    std::thread serverThread([&]() {
        broadcastServer.run(9002);
    });

    std::thread cameraThread([&]() {
        captureAndSend(broadcastServer);
    });

    serverThread.join();
    cameraThread.join();
    
    return 0;
}
