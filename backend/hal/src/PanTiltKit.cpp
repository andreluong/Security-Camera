#include "PanTiltKit.h"
#include <cstdio>
#include <cassert>

const std::string& PAN_FILE_PATH = "/dev/hat/pwm/GPIO6/";
const std::string& TILT_FILE_PATH = "/dev/hat/pwm/GPIO14/";

constexpr int PAN_MAX_ANGLE = 180;
constexpr int TILT_MAX_ANGLE = 90;
constexpr int PAN_MIN_ANGLE = 0;
constexpr int TILT_MIN_ANGLE = 0;
constexpr int PAN_PERIOD = 10;
constexpr int TILT_PERIOD = 30;

PanTiltKit::PanTiltKit() {
    panServo = std::make_unique<Microservo>(PAN_FILE_PATH, PAN_PERIOD, PAN_MIN_ANGLE, PAN_MAX_ANGLE);
    tiltServo = std::make_unique<Microservo>(TILT_FILE_PATH, TILT_PERIOD, TILT_MIN_ANGLE, TILT_MAX_ANGLE);
}

PanTiltKit::~PanTiltKit() {
    std::printf("PanTiltKit module shutdown.\n");
}

void PanTiltKit::increasePanAngle(int angle) {
    panServo->increaseAngle(angle);
}

void PanTiltKit::decreasePanAngle(int angle) {
    panServo->decreaseAngle(angle);
}

void PanTiltKit::increaseTiltAngle(int angle) {
    tiltServo->increaseAngle(angle);
}

void PanTiltKit::decreaseTiltAngle(int angle) {
    tiltServo->decreaseAngle(angle);
}