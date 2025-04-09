#include "rotary_button.h"
#include <cstdio>
#include <chrono>

RotaryButton::RotaryButton() {
    button = std::make_unique<Button>(GpioLine(BTN_GPIO_CHIP, BTN_LINE_NUM));
}

RotaryButton::~RotaryButton() {
    button->setIsRunning(false);
    std::printf("Button pressed, terminating program...\n");
}

bool RotaryButton::isPressed() {
    return button->isPressed();
}