// Low-level GPIO access using gpiod
#ifndef _GPIO_H_
#define _GPIO_H_

#include <gpiod.h>

enum GpioChip {
    GPIO_CHIP_0,
    GPIO_CHIP_1,
    GPIO_CHIP_2,
    GPIO_NUM_CHIPS // Count of chips
};

class Gpio {
public:
    Gpio();
    ~Gpio();
private:
    bool is_initialized;
};

class GpioLine {
public:
    // Opening a pin gives us a "line" that we later work with.
    //  chip: such as GPIO_CHIP_0
    //  pinNumber: such as 15
    GpioLine(GpioChip chip, int pinNumber);
    ~GpioLine();

    int waitForLineChange(struct gpiod_line_bulk* bulkEvents);
private:
    bool is_initialized;
    struct gpiod_line* line;
};

#endif