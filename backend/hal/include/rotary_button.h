#ifndef ROTARY_BUTTON_H
#define ROTARY_BUTTON_H

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <atomic>
#include <thread>
#include "gpio.h"
#include <vector>
#include <functional>
#include "PanTiltKit.h"

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

class RotaryButton {
public:
    RotaryButton();
    ~RotaryButton();

    bool isPressed();

private:
    std::atomic<bool> is_initialized;
    std::atomic<bool> is_running;
    std::atomic<bool> pressed;
    int i2c_file_desc;
    GpioLine btnLine;
    std::vector<ButtonState> states;
    ButtonState* currentState = nullptr;

    std::thread buttonThread;


    void onRelease();
    void processStateEvent(const bool isRising, ButtonStateEvent* risingEvent, ButtonStateEvent* fallingEvent);
    void processButton(); // Thread

    static const GpioChip BTN_GPIO_CHIP = GPIO_CHIP_0;
    static constexpr int BTN_LINE_NUM = 10;
};

#endif // ROTARY_BUTTON_H