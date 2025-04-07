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
        CameraFeed(PersonDetector& pd, BroadcastServer& server);
        ~CameraFeed();
        void captureAndQueueFrame();
        void dequeAndSendFrame();
        void captureAndSend();
    private:
        std::deque<cv::Mat> frameQueue;
        std::mutex frameMutex;
        std::thread captureThread;
        std::thread detectThread;
        bool isRunning;
        PersonDetector& personDetector;
        BroadcastServer& broadcastServer;

        
};