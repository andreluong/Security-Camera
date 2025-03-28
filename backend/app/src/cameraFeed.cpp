#include "cameraFeed.h"
#include <mutex>
#include <thread>

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

CameraFeed::CameraFeed(PersonDetector pd) {
    isRunning = true;
    personDetector = pd;
}

void CameraFeed::captureAndQueueFrame() {
    // Open camera
    cv::VideoCapture capture(USB_CAMERA_PORT);
    if (!capture.isOpened()) {
        std::cerr << "Error: Could not open camera!\n";
        return;
    }

    // Queue frames for the detector to process and broadcast
    cv::Mat frame;
    while (isRunning) {
        capture >> frame;
        if (frame.empty()) break;
        
        frameMutex.lock();
        frameQueue.push_front(frame);
        frameMutex.unlock();
    }
}


void CameraFeed::dequeAndSendFrame(BroadcastServer& broadcastServer) {
    while(isRunning) {
        if(!frameQueue.empty()) {
            cv::Mat frame = frameQueue.back();
    
            frameMutex.lock();
            frameQueue.pop_back();
            frameMutex.unlock();
    
            frame = personDetector.detectPeopleInFrame(frame);
            
            //broadcast to server?
            broadcastServer.sendFrame(frame);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_DELAY_MS));
    }
}