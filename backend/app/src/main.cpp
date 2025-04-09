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
#include "AudioMixer.h"
#include "Alarm.h"

int main() {
    std::printf("Starting server...\n");

    Gpio gpio;

    BroadcastServer broadcastServer;
    PersonDetector personDetector;
    CameraFeed cameraFeed(personDetector, broadcastServer);

    AudioMixer audioMixer;
    Alarm alarm(audioMixer);
    PanTiltKit panTiltKit;
    Joystick joystick(panTiltKit, alarm);
    NightLight nightLight;

    RotaryButton button;

    while(!button.isPressed()) {

    }

    std::printf("Closing server...\n");

    return 0;
}
