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
#include "joystick.h"
#include "gpio.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

int main() {
    std::cout << "Starting server\n";

    Gpio gpio;
    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector);

    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit);    

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
