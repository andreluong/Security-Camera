#include "personDetector.h"
#include <mutex>

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

PersonDetector::PersonDetector() {
    net = cv::dnn::readNetFromCaffe(modelConfig, modelWeights);
    if (net.empty()) {
        std::cerr << "Error loading model." << std::endl;
    }

    detectedPeople = 0;
}

int PersonDetector::getPeopleDetected() {
    return detectedPeople;
}

cv::Mat PersonDetector::detectPeopleInFrame(cv::Mat cameraFrame) {
    cv::Mat frame = cameraFrame;
    cv::Mat blob = cv::dnn::blobFromImage(frame, 0.007843, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5), true, false);
    net.setInput(blob);

    // Perform forward pass
    cv::Mat detections = net.forward();
    if(detections.empty()) {
        std::cerr << "Forward pass failed\n";
    }

    // Analyze detections
    int detected = 0;
    for (int i = 0; i < detections.size[2]; i++) {

        int idxConfidence[4] = {0,0,i,2};
        float confidence = detections.at<float>(idxConfidence);

        int idxClass[4] = {0,0,i,1};
        int classId = detections.at<float>(idxClass);

        int idxLeft[4] = {0,0,i,3};
        int idxTop[4] = {0,0,i,4};
        int idxRight[4] = {0,0,i,5};
        int idxBottom[4] = {0,0,i,6};

        if (confidence > MIN_CONFIDENCE && classId == PERSON_ID) {
            int left = static_cast<int>(detections.at<float>(idxLeft) * frame.cols);
            int top = static_cast<int>(detections.at<float>(idxTop) * frame.rows);
            int right = static_cast<int>(detections.at<float>(idxRight) * frame.cols);
            int bottom = static_cast<int>(detections.at<float>(idxBottom) * frame.rows);

            // Draw the bounding box
            cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0), 2);

            // Put the label text
            std::string label =  "Person: " + std::to_string(confidence);
            cv::putText(frame, label, cv::Point(left, top - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
            detected++;
        }
    }
    detectedPeople = detected;

    return frame;
}