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

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";


// Captures current frame and sends to all connections
void captureAndSend(BroadcastServer& broadcastServer) {
    cv::VideoCapture capture(USB_CAMERA_PORT);
    PersonDetector detector;
    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera!\n";
        return;
    }

    cv::Mat frame;

    while (true) {
        capture >> frame;
        if (frame.empty()) break;
        
        frame = detector.detectPeopleInFrame(frame);
        std::cout << detector.getPeopleDetected() << std::endl;

        broadcastServer.sendFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_DELAY_MS));
    }
}

int main() {
    std::cout << "Starting server\n";

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
