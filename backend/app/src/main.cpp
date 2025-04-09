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
#include "Alarm.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

int main() {
    Gpio gpio;
    std::cout << "Starting server\n";

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector, broadcastServer);

    AudioMixer audioMixer;
    Alarm alarm(audioMixer);
    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit, alarm);
    RotaryButton button;

    CommandServer commandServer = CommandServer(panTiltKit, personDetector, alarm);

    NightLight nightLight;
    while(!button.isPressed()) {

    }
    //Just testing sound


    //commandThread.join();
    // cameraSendThread.join();
    // cameraFeedThread.join();   
    //broadcastThread.join();

    std::cout << "Closing server\n";

    return 0;
}
