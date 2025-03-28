#pragma once

#include "Microservo.h"
#include <memory>

constexpr int ANGLE_STEP = 5;

class PanTiltKit {
public:
    PanTiltKit(std::unique_ptr<Microservo> pan, std::unique_ptr<Microservo> tilt);

    void increasePanAngle(int angle = ANGLE_STEP);
    void decreasePanAngle(int angle = ANGLE_STEP);

    void increaseTiltAngle(int angle = ANGLE_STEP);
    void decreaseTiltAngle(int angle = ANGLE_STEP);

private:
    std::unique_ptr<Microservo> panServo;
    std::unique_ptr<Microservo> tiltServo;
};