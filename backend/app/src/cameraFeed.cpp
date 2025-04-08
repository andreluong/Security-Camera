#include "cameraFeed.h"
#include <mutex>
#include <thread>
#include <iostream>

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

CameraFeed::CameraFeed(PersonDetector& pd, BroadcastServer& server): isRunning(true), personDetector(pd), broadcastServer(server) {
    captureThread = std::thread(&CameraFeed::captureAndQueueFrame, this);
    detectThread = std::thread(&CameraFeed::dequeAndSendFrame, this);
}

CameraFeed::~CameraFeed() {
    isRunning = false;
    detectThread.join();
    captureThread.join();
    std::cout << "Camera turned off" << std::endl;
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
        broadcastServer.sendFrame(frame);
        frameMutex.unlock();
    }
    capture.release();
    std::cout << "Camera capture turned off" << std::endl;
}

void CameraFeed::dequeAndSendFrame() {
    while(isRunning) {
        if(!frameQueue.empty()) {
            cv::Mat frame = frameQueue.back();
    
            frameMutex.lock();
            frameQueue.pop_back();
            frameMutex.unlock();
    
            frame = personDetector.detectPeopleInFrame(frame);
            
            // //broadcast to server?
            // broadcastServer.sendFrame(frame);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_DELAY_MS));
    }
}