#include "cameraFeed.h"
#include <mutex>

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

CameraFeed::CameraFeed(std::deque<cv::Mat>* frames) {
    frameQueue = frames;
}

// Thread function?
void CameraFeed::queueFrame() {
    cv::VideoCapture capture(USB_CAMERA_PORT);
    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera!\n";
        return;
    }

    // Queue frames for the detector to process and broadcast
    cv::Mat frame;
    while (true) {
        capture >> frame;
        if (frame.empty()) break;
        
        frameQueue->push_front(frame);
    }
}