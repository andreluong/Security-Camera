#ifndef PERSON_DETECTOR_H
#define PERSON_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <atomic>

#define PERSON_ID 15
#define MIN_CONFIDENCE 0.2
#define SCALE_FACTOR 0.007843

class PersonDetector {
    public:
        PersonDetector();
        PersonDetector(std::deque<cv::Mat>* frames);
        ~PersonDetector() = default;
        int getPeopleDetected();
        void detectPeopleInFrame();
        cv::Mat detectPeopleInFrame(cv::Mat cameraFrame);
    private:
        std::atomic<int> detectedPeople;
        cv::dnn::Net net;
        std::deque<cv::Mat>* frames;
};

#endif