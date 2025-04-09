#pragma once

#include <thread>
#include <functional>
#include <atomic>
#include "gpio.h"

struct ButtonState;

struct ButtonStateEvent {
    ButtonState* nextState = nullptr;
    std::function<void()> action = nullptr;

    ButtonStateEvent(ButtonState* next, std::function<void()> act) 
        : nextState(std::move(next)), action(std::move(act)) {}
};

struct ButtonState {
    ButtonStateEvent rising;
    ButtonStateEvent falling;
};

class Button {
public:
    Button(GpioLine line);
    ~Button();

    bool isPressed();
    void setPressed(bool value);

    bool getIsRunning();
    void setIsRunning(bool value);

private:
    std::atomic<bool> isRunning;
    std::atomic<bool> pressed;    
    std::thread buttonThread;
    GpioLine btnLine;
    std::vector<ButtonState> states;
    ButtonState* currentState = nullptr;

    void onRelease();
    void processStateEvent(const bool isRising, ButtonStateEvent* risingEvent, ButtonStateEvent* fallingEvent);
    void processButton();
    void doNothing();
};