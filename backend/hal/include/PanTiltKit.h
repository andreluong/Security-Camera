#pragma once

#include "Microservo.h"
#include <memory>

constexpr int ANGLE_STEP = 10;

class PanTiltKit {
public:
    PanTiltKit();
    ~PanTiltKit();

    void increasePanAngle(int angle = ANGLE_STEP);
    void decreasePanAngle(int angle = ANGLE_STEP);

    void increaseTiltAngle(int angle = ANGLE_STEP);
    void decreaseTiltAngle(int angle = ANGLE_STEP);

private:
    bool is_initialized;

    std::unique_ptr<Microservo> panServo;
    std::unique_ptr<Microservo> tiltServo;
};