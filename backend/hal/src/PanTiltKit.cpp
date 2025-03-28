#include "PanTiltKit.h"

PanTiltKit::PanTiltKit(std::unique_ptr<Microservo> pan, std::unique_ptr<Microservo> tilt)
    : panServo(std::move(pan)), tiltServo(std::move(tilt)) {}

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