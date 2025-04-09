#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <atomic>
#include <thread>
#include "gpio.h"
#include "PanTiltKit.h"
#include "Alarm.h"
#include "Button.h"
#include <memory>

enum class JoystickDirection {
    IDLE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    PRESSED
};

class Joystick {
public:
    Joystick(PanTiltKit& kit, Alarm& a);
    ~Joystick();

private:
    std::atomic<bool> is_running;
    int i2c_file_desc;
    PanTiltKit& panTiltKit;
    Alarm& alarm;

    std::thread joystickThread;
    std::unique_ptr<Button> button;

    void processDirection(); // Thread
    JoystickDirection getDirection();
    int getX();
    int getY();

    // Constants
    static constexpr int minThresh = 400;
    static constexpr int maxThresh = 1100;
    static constexpr int coordIdle = 0;
    static constexpr int coordMin = -100;
    static constexpr int coordMax = 100;

    static const GpioChip JOYSTICK_GPIO_CHIP = GPIO_CHIP_2;
    static constexpr int JOYSTICK_LINE_NUM = 15;
};

#endif