#include <iostream>
#include "broadcastServer.h"
#include "personDetector.h"
#include "cameraFeed.h"
#include "CommandServer.h"
#include "PanTiltKit.h"
#include "joystick.h"
#include "rotary_button.h"
#include "gpio.h"
#include "NightLight.h"

#define USB_CAMERA_PORT 3
#define CAMERA_DELAY_MS 30

const std::string modelWeights = "models/MobileNetSSD.caffemodel";
const std::string modelConfig = "models/MobileNetSSD.prototxt";

int main() {
    std::printf("Starting server...\n");

    Gpio gpio;

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector, broadcastServer);

    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit);
    NightLight nightLight;

    CommandServer commandServer = CommandServer(panTiltKit, personDetector, cameraFeed);

    RotaryButton button;

    while(!button.isPressed()) {

    }

    std::printf("Closing server...\n");

    return 0;
}
