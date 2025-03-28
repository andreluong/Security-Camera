#ifndef JOYSTICK_BUTTON_H
#define JOYSTICK_BUTTON_H

#include <gpiod.h>
#include <thread>
#include <atomic>

class JoystickButton {
public:
    JoystickButton(const char* chipPath = "/dev/gpiochip2", unsigned int line = 15);
    ~JoystickButton();

    void startListening();
    void stopListening();
    bool isPressed();

private:
    const char* chipPath;
    unsigned int lineNumber;
    std::atomic<bool> running;
    std::atomic<bool> pressed;

    void listenerLoop();
    long getCurrentTimeMs() const;    

    std::thread listenerThread;
    struct gpiod_chip* chip = nullptr;
    struct gpiod_line* buttonLine = nullptr;
};

#endif // JOYSTICK_BUTTON_H