#include "Button.h"
#include <cstdio>
#include <chrono>

Button::Button(GpioLine line) : isRunning(true), pressed(false), btnLine(line) {
    // Setup the states
    printf("Initing button\n");
    states.reserve(2);
    ButtonState one = {ButtonStateEvent(&states[0], nullptr), ButtonStateEvent(&states[1], nullptr)};
    ButtonState two = {ButtonStateEvent(&states[0], [this]() { onRelease(); }), ButtonStateEvent(&states[1], nullptr)};
    states.push_back(one);
    states.push_back(two);
    currentState = &states[0];
    buttonThread = std::thread(&Button::processButton, this);
    printf("reached end\n");
}

Button::~Button() {
    isRunning = false;
    pthread_cancel(buttonThread.native_handle()); // Must be cancelled due to hanging
    if (buttonThread.joinable()) buttonThread.join();
}

void Button::onRelease() {
    pressed = true;
}

bool Button::isPressed() {
    return pressed.load();
}

void Button::setPressed(bool value) {
    pressed = value;
}

bool Button::getIsRunning() {
    return isRunning.load();
}

void Button::setIsRunning(bool value) {
    isRunning = value;
}

void Button::processStateEvent(const bool isRising, ButtonStateEvent* risingEvent, ButtonStateEvent* fallingEvent) {
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

void Button::processButton() {
    while (isRunning) {
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
