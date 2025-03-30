#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include "personDetector.h"
#include "broadcastServer.h"

class CameraFeed {
    public:
        CameraFeed();
        CameraFeed(PersonDetector& pd);
        ~CameraFeed() = default;
        void captureAndQueueFrame();
        void dequeAndSendFrame(BroadcastServer& broadcastServer);
        void captureAndSend();
    private:
        std::deque<cv::Mat> frameQueue;
        std::mutex frameMutex;
        bool isRunning;
        PersonDetector& personDetector;

        
};