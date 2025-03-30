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
