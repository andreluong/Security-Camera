#include "rotary_button.h"
#include <cstdio>
#include "i2cHelpers.h"
#include <chrono>
#include <cassert>
#include <iostream>
#include <pthread.h>

RotaryButton::RotaryButton() : is_running(true), pressed(false), btnLine(GpioLine(BTN_GPIO_CHIP, BTN_LINE_NUM)) {

    // Setup the states
    states.reserve(2);
    ButtonState one = {ButtonStateEvent(&states[0], nullptr), ButtonStateEvent(&states[1], nullptr)};
    ButtonState two = {ButtonStateEvent(&states[0], [this]() { onRelease(); }), ButtonStateEvent(&states[1], nullptr)};
    states.push_back(one);
    states.push_back(two);
    currentState = &states[0];

    buttonThread = std::thread(&RotaryButton::processButton, this);
}

RotaryButton::~RotaryButton() {
    is_running = false;
    std::cout << "Button pressed, shutting down" << std::endl;
    pthread_cancel(buttonThread.native_handle()); // Must be cancelled due to hanging
    if (buttonThread.joinable()) buttonThread.join();
}

void RotaryButton::onRelease() {
    pressed = true;
}

bool RotaryButton::isPressed() {
    return pressed.load();
}

void RotaryButton::processStateEvent(const bool isRising, ButtonStateEvent* risingEvent, ButtonStateEvent* fallingEvent) {
    ButtonStateEvent* pStateEvent = nullptr;

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

void RotaryButton::processButton() {
    while (is_running.load()) {
        struct gpiod_line_bulk bulkEvents;
        int numEvents = btnLine.waitForLineChange(&bulkEvents);

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