#ifndef ROTARY_BUTTON_H
#define ROTARY_BUTTON_H

#include "Button.h"
#include <memory>

class RotaryButton {
public:
    RotaryButton();
    ~RotaryButton();

    bool isPressed();

private:
    std::unique_ptr<Button> button;

    static const GpioChip BTN_GPIO_CHIP = GPIO_CHIP_0;
    static constexpr int BTN_LINE_NUM = 10;
};

#endif