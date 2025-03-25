#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "i2cHelpers.h"
#include "lightSensor.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";


// Captures current frame and sends to all connections
void captureAndSend(BroadcastServer& broadcastServer) {
    cv::VideoCapture capture(USB_CAMERA_PORT);
    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera!\n";
        return;
    }

    cv::Mat frame;
    cv::dnn::Net net = cv::dnn::readNetFromCaffe(modelConfig, modelWeights);
    if (net.empty()) {
        std::cerr << "Error loading model." << std::endl;
    }

    while (true) {
        capture >> frame;
        // cv::rectangle(frame, cv::Point(30, 30), cv::Point(50, 50), cv::Scalar(0, 0, 255), 3);
        if (frame.empty()) break;

        // Prepare input for the model
        cv::Mat blob = cv::dnn::blobFromImage(frame, 0.007843, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5), true, false);
        net.setInput(blob);

        // Perform forward pass
        cv::Mat detections = net.forward();

        // Parse results
        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
        
        
        for (int i = 0; i < detectionMat.rows; i++) {
            // float confidence = detectionMat.at<float>(i, 2);

            // // if (confidence == 0) {  // Confidence threshold
            // //     continue;
            // // }
            int objectClass = (int)detectionMat.at<float>(i, 1);
            if (objectClass == 0) {  // Person class
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                // Draw bounding box
                cv::rectangle(frame, cv::Point(xLeftBottom, yLeftBottom), cv::Point(xRightTop, yRightTop), cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, "Person", cv::Point(xLeftBottom, yLeftBottom - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
            }
        }

        broadcastServer.sendFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_DELAY_MS));
    }
}

int main() {
    BroadcastServer broadcastServer;
    LightSensor sensor;
     
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
