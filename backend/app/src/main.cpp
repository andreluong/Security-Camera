#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"

#include "Microservo.h"
#include "PanTiltKit.h"
#include <memory>

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

// TODO: Remove after integration
void testPanTiltKit() {
    std::cout << "Starting\n";

    auto pan = std::make_unique<Microservo>("/dev/hat/pwm/GPIO6/", 0, 180);
    auto tilt = std::make_unique<Microservo>("/dev/hat/pwm/GPIO14/", 0, 90);
    PanTiltKit panTiltKit(std::move(pan), std::move(tilt));

    panTiltKit.increasePanAngle(180);
    panTiltKit.increaseTiltAngle(90);

    panTiltKit.decreasePanAngle(180);
    panTiltKit.decreaseTiltAngle(90);

    std::cout << "Done\n";
}

int main() {
    BroadcastServer broadcastServer;

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
