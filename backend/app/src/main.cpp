#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <thread>
#include "broadcastServer.h"
#include "personDetector.h"
#include "i2cHelpers.h"
#include "cameraFeed.h"
#include "CommandServer.h"
#include "Microservo.h"
#include "PanTiltKit.h"
#include "PlaySound.h"
#include <memory>
#include "joystick.h"
#include "rotary_button.h"
#include "gpio.h"
#include "NightLight.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

int main() {
    std::cout << "Starting server\n";
    Gpio gpio;

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector, broadcastServer);

    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit);
    RotaryButton button;

    CommandServer commandServer = CommandServer(panTiltKit, personDetector);

    NightLight nightLight;

    while(!button.isPressed()) {

    }
    //Just testing sound
    // Audio::init();
    // Sound sound;
    // sound.playSound();

    //commandThread.join();
    // cameraSendThread.join();
    // cameraFeedThread.join();   
    //broadcastThread.join();

    std::cout << "Closing server\n";

    return 0;
}
