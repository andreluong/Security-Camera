#include "gpio.h"
#include <string>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <gpiod.h>

static struct gpiod_chip* s_openGpiodChips[GPIO_NUM_CHIPS];

static const std::string chipNames[] = {
    "gpiochip0",
    "gpiochip1",
    "gpiochip2",
};

Gpio::Gpio() : is_initialized(true) {
    for (int i = 0; i < GPIO_NUM_CHIPS; i++) {
        s_openGpiodChips[i] = gpiod_chip_open_by_name(chipNames[i].c_str());
        if (!s_openGpiodChips[i]) {
            perror("GPIO Initializing: Unable to open GPIO chip");
            exit(EXIT_FAILURE);
        }
    }
}

Gpio::~Gpio() {
    assert(is_initialized);
    for (int i = 0; i < GPIO_NUM_CHIPS; i++) {
        gpiod_chip_close(s_openGpiodChips[i]);
        if (!s_openGpiodChips[i]) {
            perror("GPIO Disabling: Unable to close GPIO chip");
            exit(EXIT_FAILURE);
        }
    }
    is_initialized = false;
}

// Opening a pin gives us a "line" that we later work with.
//  chip: such as GPIO_CHIP_0
//  pinNumber: such as 15
GpioLine::GpioLine(GpioChip chip, int pinNumber) : is_initialized(true) {
    struct gpiod_chip* gpiodChip = s_openGpiodChips[chip];
    line = gpiod_chip_get_line(gpiodChip, pinNumber);
    if (!line) {
        perror("Unable to get GPIO line");
        exit(EXIT_FAILURE);
    }
}

GpioLine::~GpioLine() {
    assert(is_initialized);
    gpiod_line_release(line);
    line = nullptr;
    is_initialized = false;
}

// Returns the number of events
int GpioLine::waitForLineChange(struct gpiod_line_bulk *bulkEvents) {
    assert(is_initialized);

    // Source: https://people.eng.unimelb.edu.au/pbeuchat/asclinic/software/building_block_gpio_encoder_counting.html   
    struct gpiod_line_bulk bulkWait;
    gpiod_line_bulk_init(&bulkWait);
    
    // NOTE: Add more lines if needed
    gpiod_line_bulk_add(&bulkWait, line);
    
    gpiod_line_request_bulk_both_edges_events(&bulkWait, "Event Waiting");

    int result = gpiod_line_event_wait_bulk(&bulkWait, NULL, bulkEvents);
    if ( result == -1) {
        perror("Error waiting on lines for event waiting");
        exit(EXIT_FAILURE);
    }

    int numEvents = gpiod_line_bulk_num_lines(bulkEvents);
    return numEvents;
}
