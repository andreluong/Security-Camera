#include "joystick.h"
#include <cstdio>
#include "i2cHelpers.h"
#include <chrono>
#include <cassert>
#include <pthread.h>

// Construct joystick with thread
Joystick::Joystick(PanTiltKit& kit, Alarm& a) : is_running(true), i2c_file_desc(0), panTiltKit(kit), alarm(a) {
    i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    joystickThread = std::thread(&Joystick::processDirection, this);
    button = std::make_unique<Button>(GpioLine(JOYSTICK_GPIO_CHIP, JOYSTICK_LINE_NUM));
}

// Terminate thread when object is destroyed
Joystick::~Joystick() {
    button->setIsRunning(false);
    is_running = false;
    if (joystickThread.joinable()) joystickThread.join();
    close(i2c_file_desc);
    printf("Joystick module shutdown.\n");
}

// Changes servo angles based on direction
// Play alarm sound when pressed
void Joystick::processDirection() {
    while (is_running.load()) {
        auto direction = getDirection();
        switch (direction) {
            case JoystickDirection::UP: {
                panTiltKit.increaseTiltAngle();
                std::printf("Joystick Direction: UP\n");
                break;
            }
            case JoystickDirection::DOWN: {
                panTiltKit.decreaseTiltAngle();
                std::printf("Joystick Direction: DOWN\n");
                break;
            }
            case JoystickDirection::LEFT: {
                panTiltKit.increasePanAngle();
                std::printf("Joystick Direction: LEFT\n");
                break;
            }
            case JoystickDirection::RIGHT: {
                panTiltKit.decreasePanAngle();
                std::printf("Joystick Direction: RIGHT\n");
                break;
            }
            case JoystickDirection::PRESSED: {
                std::printf("Joystick Direction: PRESSED\n");
                alarm.alert();
                break;
            }
            default: break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int Joystick::getX() {
    i2cOperations::write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, JOYSTICK_X);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    uint16_t raw_read = i2cOperations::read_i2c_reg16(i2c_file_desc, REG_DATA);
    uint16_t scaled_read = i2cOperations::swap_and_scale(raw_read);

    if (scaled_read >= maxThresh) return coordMax;
    if (scaled_read <= minThresh) return coordMin;
    return coordIdle;
}

int Joystick::getY() {
    i2cOperations::write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, JOYSTICK_Y);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    uint16_t raw_read = i2cOperations::read_i2c_reg16(i2c_file_desc, REG_DATA);
    uint16_t scaled_read = i2cOperations::swap_and_scale(raw_read);

    if (scaled_read >= maxThresh) return coordMin;
    if (scaled_read <= minThresh) return coordMax;
    return coordIdle;
}

JoystickDirection Joystick::getDirection() {
    if (button->isPressed()) {
        button->setPressed(false); // Reset after reading
        return JoystickDirection::PRESSED;
    }

    int xVal = getX();
    if (xVal == coordMax) return JoystickDirection::RIGHT;
    if (xVal == coordMin) return JoystickDirection::LEFT;

    int yVal = getY();
    if (yVal == coordMax) return JoystickDirection::UP;
    if (yVal == coordMin) return JoystickDirection::DOWN;

    return JoystickDirection::IDLE;
}
