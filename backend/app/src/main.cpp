#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "personDetector.h"
#include "i2cHelpers.h"
#include "lightSensor.h"

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
    BroadcastServer broadcastServer;
     
    std::thread serverThread([&]() {
        broadcastServer.run(9002);
    });

    std::thread cameraThread([&]() {
        captureAndSend(broadcastServer);
    });

    // std::thread sensorThread([&]() {
    //     sensor.getSamples();
    // });

    // sensorThread.join();

    cameraThread.join();
    serverThread.join();

    
    return 0;
}
