#include "JoystickButton.h"
#include "PeriodTimer.h"
#include <iostream>
#include <unistd.h>
#include <chrono>

JoystickButton::JoystickButton(const char* chipPath, unsigned int line)
    : chipPath(chipPath), lineNumber(line), running(false), pressed(false) {
    
    chip = gpiod_chip_open(chipPath);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return;
    }

    buttonLine = gpiod_chip_get_line(chip, lineNumber);
    if (!buttonLine) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        chip = nullptr;
        return;
    }

    if (gpiod_line_request_input(buttonLine, "joystick_btn") < 0) {
        perror("Failed to request button line as input");
        gpiod_chip_close(chip);
        chip = nullptr;
        buttonLine = nullptr;
        return;
    }
}

JoystickButton::~JoystickButton() {
    stopListening();
    if (buttonLine) {
        gpiod_line_release(buttonLine);
    }
    if (chip) {
        gpiod_chip_close(chip);
    }
}

void JoystickButton::startListening() {
    if (!running && buttonLine) {
        running = true;
        listenerThread = std::thread(&JoystickButton::listenerLoop, this);
        listenerThread.detach(); // Background thread
    }
}

void JoystickButton::stopListening() {
    running = false;
}

bool JoystickButton::isPressed(){
    if (pressed) {
        pressed = false; // Reset after reading
        return true;
    }
    return false;
}

void JoystickButton::listenerLoop() {
    int lastState = 1; // Assume HIGH
    long lastPressTime = 0;

    while (running) {
        if (!buttonLine) continue;

        int currentState = gpiod_line_get_value(buttonLine);
        long currentTime = getCurrentTimeMs();

        // Used externally to suppress air drum triggering
        if (currentState == 0) {
            pressed = true;
        }

        if (lastState == 1 && currentState == 0) {
            if (currentTime - lastPressTime > 200) { // 200ms debounce
                lastPressTime = currentTime;
                lastState = currentState;
            }
        }
        
        lastState = currentState;
        usleep(100000); // 100ms delay
    }
}


long JoystickButton::getCurrentTimeMs() const {
    return PeriodTimer::getCurrentTimeMs();
}