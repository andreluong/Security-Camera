#include "JoystickButton.h"
#include "PeriodTimer.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <cassert>

const GpioChip JOYSTICK_GPIO_CHIP = GPIO_CHIP_2;
constexpr int JOYSTICK_LINE_NUM = 15;

JoystickButton::JoystickButton() : is_initialized(true), running(true), pressed(false), 
    joystickLine(GpioLine(JOYSTICK_GPIO_CHIP, JOYSTICK_LINE_NUM))
{
    // Setup the states
    states.reserve(2);
    State one = {StateEvent(&states[0], nullptr), StateEvent(&states[1], nullptr)};
    State two = {StateEvent(&states[0], [this]() { onRelease(); }), StateEvent(&states[1], nullptr)};
    states.push_back(one);
    states.push_back(two);
    currentState = &states[0];

    listenerThread = std::thread(&JoystickButton::processButton, this);
}

JoystickButton::~JoystickButton() {
    assert(is_initialized);
    running = false;
    if (listenerThread.joinable()) listenerThread.join();
    is_initialized = false;
}

bool JoystickButton::isPressed(){
    if (pressed) {
        pressed = false; // Reset after reading
        printf("pressed\n");
        return true;
    }
    return false;
}

void JoystickButton::onRelease(void) {
    pressed = true;
}


// Read button input; will definitely refactor
void JoystickButton::processStateEvent(const bool isRising, StateEvent* risingEvent, StateEvent* fallingEvent) {
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

void JoystickButton::processButton() {
    // int lastState = 1; // Assume HIGH
    // long lastPressTime = 0;

    while (running) {
        struct gpiod_line_bulk bulkEvents;
        int numEvents = joystickLine.waitForLineChange(&bulkEvents);

        for (int i = 0; i < numEvents; i++) {
            struct gpiod_line* line_handle = gpiod_line_bulk_get_line(&bulkEvents, i);

            // Get line events
            struct gpiod_line_event event;
            if (gpiod_line_event_read(line_handle, &event) == -1) {
                perror("Line Event");
                exit(EXIT_FAILURE);
            }

            // Run the state machine
            bool isRising = event.event_type == GPIOD_LINE_EVENT_RISING_EDGE;
            processStateEvent(isRising, &currentState->rising, &currentState->falling);
        }
    }
}
