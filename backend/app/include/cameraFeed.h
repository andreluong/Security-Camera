#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>


class CameraFeed {
    public:
        CameraFeed(std::deque<cv::Mat>* frames);
        ~CameraFeed() = default;
        void queueFrame();
    private:
        std::deque<cv::Mat>* frameQueue;
};