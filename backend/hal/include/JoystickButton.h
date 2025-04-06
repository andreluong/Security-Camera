#ifndef JOYSTICK_BUTTON_H
#define JOYSTICK_BUTTON_H

#include <gpiod.h>
#include <thread>
#include <atomic>
#include "gpio.h"
#include <vector>
#include <functional>

struct State;

struct StateEvent {
    State* nextState = nullptr;
    std::function<void()> action = nullptr;

    StateEvent(State* next, std::function<void()> act): nextState(std::move(next)), action(std::move(act)) {}
};

struct State {
    StateEvent rising;
    StateEvent falling;
};

class JoystickButton {
public:
    JoystickButton();
    ~JoystickButton();

    bool isPressed();

private:
    std::atomic<bool> is_initialized;
    std::atomic<bool> running;
    std::atomic<bool> pressed;
    std::thread listenerThread;

    GpioLine joystickLine;

    std::vector<State> states;
    void onRelease();

    State* currentState = nullptr;

    void processStateEvent(const bool isRising, StateEvent* risingEvent, StateEvent* fallingEvent);
    void processButton();
};

#endif // JOYSTICK_BUTTON_H