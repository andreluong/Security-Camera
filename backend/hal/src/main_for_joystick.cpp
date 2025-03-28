#include "joystick.h"
#include "JoystickButton.h"
#include "PlaySound.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        Audio::init();
        Sound Sound; 
        Joystick joystick;
        JoystickButton joystickBtn;

        joystickBtn.startListening();

        while (true) {
            // uint16_t x = Joystick.readX();
            // uint16_t y = Joystick.readY();

            // std::cout << "Joystick X: " << x << ", Y: " << y << " -> ";
            bool pressed = joystickBtn.isPressed();
            JoystickDirection dir = joystick.getDirection();

            switch (dir) {
            case JoystickDirection::UP:
                std::cout << "Joystick Direction: UP" << std::endl;
                break;
            case JoystickDirection::DOWN:
                std::cout << "Joystick Direction: DOWN" << std::endl;
                break;
            case JoystickDirection::LEFT:
                std::cout << "Joystick Direction: LEFT" << std::endl;
                break;
            case JoystickDirection::RIGHT:
                std::cout << "Joystick Direction: RIGHT" << std::endl;
                break;
            case JoystickDirection::IDLE:
                std::cout << "Joystick Direction: IDLE" << std::endl;
                break;
        }
            
            
        if (pressed) {
            Sound.playSound();
            std::cout << "Joystick is PRESSED!";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        joystickBtn.stopListening(); // Optional cleanup (not required with detached thread)
    } catch (...) {
        std::cerr << "An error occurred while initializing the joystick or button." << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}