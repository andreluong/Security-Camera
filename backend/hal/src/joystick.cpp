#include "joystick.h"
#include <cstdio>
#include "i2cHelpers.h"
#include <chrono>
#include <cassert>
#include <pthread.h>

// Construct joystick with thread
Joystick::Joystick(PanTiltKit& kit, Alarm& a) : is_initialized(true), is_running(true), pressed(false), i2c_file_desc(0), 
    panTiltKit(kit), alarm(a), joystickLine(GpioLine(JOYSTICK_GPIO_CHIP, JOYSTICK_LINE_NUM)) 
{
    i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    // Setup the states
    states.reserve(2);
    State one = {StateEvent(&states[0], nullptr), StateEvent(&states[1], nullptr)};
    State two = {StateEvent(&states[0], [this]() { onRelease(); }), StateEvent(&states[1], nullptr)};
    states.push_back(one);
    states.push_back(two);
    currentState = &states[0];
    
    joystickThread = std::thread(&Joystick::processDirection, this);
    buttonThread = std::thread(&Joystick::processButton, this);
}

// Terminate thread when object is destroyed
Joystick::~Joystick() {
    assert(is_initialized);
    is_running = false;
    pthread_cancel(buttonThread.native_handle()); // Must be cancelled due to hanging
    if (joystickThread.joinable()) joystickThread.join();
    if (buttonThread.joinable()) buttonThread.join();
    close(i2c_file_desc);
    is_initialized = false;
    printf("Joystick module shutdown.\n");
}

// Changes servo angles based on direction
// TODO: Add pan/tilt kit
// TODO: Include alarm for press
void Joystick::processDirection() {
    while (is_running) {
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
    assert(is_initialized);

    if (pressed) {
        pressed = false; // Reset after reading
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

void Joystick::onRelease() {
    pressed = true;
}

void Joystick::processStateEvent(const bool isRising, StateEvent* risingEvent, StateEvent* fallingEvent) {
    StateEvent* pStateEvent = nullptr;

    // Change state events
    if (isRising) {
        pStateEvent = risingEvent;
    } else {
        pStateEvent = fallingEvent;
    }

    // Execute action if exists
    if (pStateEvent->action != nullptr) {
        pStateEvent->action();
    }

    currentState = pStateEvent->nextState;
}

void Joystick::processButton() {
    while (is_running) {
        struct gpiod_line_bulk bulkEvents;
        int numEvents = joystickLine.waitForLineChange(&bulkEvents);

        for (int i = 0; i < numEvents; i++) {
            struct gpiod_line* line_handle = gpiod_line_bulk_get_line(&bulkEvents, i);
            
            // Get line events
            struct gpiod_line_event event;
            if (gpiod_line_event_read(line_handle, &event) == 0) {
                // Run the state machine
                bool isRising = event.event_type == GPIOD_LINE_EVENT_RISING_EDGE;
                processStateEvent(isRising, &currentState->rising, &currentState->falling);
            } else {
                perror("Line Event Read.\n");
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
