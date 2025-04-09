#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include "personDetector.h"
#include "broadcastServer.h"
#include <atomic>

class CameraFeed {
    public:
        CameraFeed();
        CameraFeed(PersonDetector& pd, BroadcastServer& server);
        ~CameraFeed();
        void captureAndQueueFrame();
        void dequeAndSendFrame();
        void captureAndSend();
        void toggle();
    private:
        std::deque<cv::Mat> frameQueue;
        std::mutex frameMutex;
        std::thread captureThread;
        std::thread detectThread;
        std::atomic<bool> isRunning;
        PersonDetector& personDetector;
        BroadcastServer& broadcastServer;
        std::atomic<bool> toggleProcessedView;   
};