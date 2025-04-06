#include "joystick.h"
#include <cstdio>
#include "i2cHelpers.h"
#include <chrono>
#include <cassert>

// Construct joystick with thread
// TODO: Inject pan/tilt kit
Joystick::Joystick() : is_initialized(true), is_running(true), i2c_file_desc(0) {
    i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    joystickThread = std::thread(&Joystick::processDirection, this);
}

// Terminate thread when object is destroyed
Joystick::~Joystick() {
    assert(is_initialized);
    is_running = false;
    if (joystickThread.joinable()) joystickThread.join();
    close(i2c_file_desc);
    is_initialized = false;
    printf("Joystick module shutdown.");
}

// Changes servo angles based on direction
// TODO: Add pan/tilt kit
void Joystick::processDirection() {
    while (is_running) {
        auto direction = getDirection();
        switch (direction) {
            case JoystickDirection::UP: {
                std::printf("Joystick Direction: UP\n");
                break;
            }
            case JoystickDirection::DOWN: {
                std::printf("Joystick Direction: DOWN\n");
                break;
            }
            case JoystickDirection::LEFT: {
                std::printf("Joystick Direction: LEFT\n");
                break;
            }
            case JoystickDirection::RIGHT: {
                std::printf("Joystick Direction: RIGHT\n");
                break;
            }
            default: break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
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
    assert(is_initialized);

    int xVal = getX();
    if (xVal == coordMax) return JoystickDirection::RIGHT;
    if (xVal == coordMin) return JoystickDirection::LEFT;

    int yVal = getY();
    if (yVal == coordMax) return JoystickDirection::UP;
    if (yVal == coordMin) return JoystickDirection::DOWN;

    return JoystickDirection::IDLE;
}
