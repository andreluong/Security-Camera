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
        if (frame.empty()) break;
        
        // Prepare input for the model
        cv::Mat blob = cv::dnn::blobFromImage(frame, 0.007843, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5), true, false);
        net.setInput(blob);

        // Perform forward pass
        cv::Mat detections = net.forward();
        if(detections.empty()) {
            std::cerr << "Forward pass failed\n";
        }

        std::cout << "detections shape: " << detections.size << std::endl;
        for (int i = 0; i < detections.size[2]; i++) {

            int idx[4] = {0,0,i,2};
            float confidence = detections.at<float>(idx);

            int l[4] = {0,0,i,3};
            int t[4] = {0,0,i,4};
            int r[4] = {0,0,i,5};
            int b[4] = {0,0,i,6};
            if (confidence > 0.2) {
                int left = static_cast<int>(detections.at<float>(l) * frame.cols);
                int top = static_cast<int>(detections.at<float>(t) * frame.rows);
                int right = static_cast<int>(detections.at<float>(r) * frame.cols);
                int bottom = static_cast<int>(detections.at<float>(b) * frame.rows);

                // Draw the bounding box
                cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0), 2);

                // Put the label text
                std::string label =  "Person: " + std::to_string(confidence);
                cv::putText(frame, label, cv::Point(left, top - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
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
