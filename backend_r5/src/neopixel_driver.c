// R5 NeoPixel Controller (Shared Memory Version)
// Reads LED colors from shared memory and drives NeoPixel strip

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>
#include "sharedDataLayout.h"

#define NEO_NUM_LEDS 8
#define SHARED_MEM 0x00000000
static void *pSharedMem = (void *) SHARED_MEM;

// NeoPixel Timing Delays
#define NEO_ONE_ON_NS  700
#define NEO_ONE_OFF_NS 600
#define NEO_ZERO_ON_NS 350
#define NEO_ZERO_OFF_NS 800
#define NEO_RESET_NS   60000

volatile int junk_delay = 0;
#define DELAY_350_NS() {}
#define DELAY_600_NS() {for (junk_delay=0; junk_delay<9 ;junk_delay++);}
#define DELAY_700_NS() {for (junk_delay=0; junk_delay<16 ;junk_delay++);}
#define DELAY_800_NS() {for (junk_delay=0; junk_delay<23 ;junk_delay++);}
#define DELAY_NS(ns) do {int target = k_cycle_get_32() + k_ns_to_cyc_near32(ns); \
	while(k_cycle_get_32() < target); } while(false)

#define NEO_DELAY_ONE_ON()     DELAY_700_NS()
#define NEO_DELAY_ONE_OFF()    DELAY_600_NS()
#define NEO_DELAY_ZERO_ON()    DELAY_350_NS()
#define NEO_DELAY_ZERO_OFF()   DELAY_800_NS()
#define NEO_DELAY_RESET()      { DELAY_NS(NEO_RESET_NS); }

#define LED0_NODE DT_ALIAS(joy0)
#define BTN0_NODE DT_ALIAS(btn0)
#define NEOPIXEL_NODE DT_ALIAS(neopixel)

static const struct gpio_dt_spec joy = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
static const struct gpio_dt_spec neopixel = GPIO_DT_SPEC_GET(NEOPIXEL_NODE, gpios);

static void initialize_gpio(const struct gpio_dt_spec *pPin, int direction) {
	if (!gpio_is_ready_dt(pPin)) {
		printf("ERROR: GPIO not ready\n");
		exit(EXIT_FAILURE);
	}
	if (gpio_pin_configure_dt(pPin, direction) < 0) {
		printf("ERROR: Failed to configure GPIO\n");
		exit(EXIT_FAILURE);
	}
}

void write_led_color(int index, uint32_t color) {
    int offset = SHARED_RGB_OFFSET + (index * 4);
    volatile uint32_t* pAddr = (volatile uint32_t *)((volatile uint8_t *)pSharedMem + offset);
    *pAddr = color;
}

int main(void) {
	//printf("R5 NeoPixel Shared Memory Driver\n");
	initialize_gpio(&neopixel, GPIO_OUTPUT_ACTIVE);
	initialize_gpio(&btn, GPIO_INPUT);
	initialize_gpio(&joy, GPIO_INPUT);

	//printf("Contents of Shared Memory ATCM:\n");
	for (int i = 0; i < END_MEMORY_OFFSET; i++) {
		char* addr = (char*)pSharedMem + i;
		printf("0x%08x = %2x (%c)\n", (uint32_t) addr, *addr, *addr);
	}

	MEM_UINT32(IS_BUTTON_PRESSED_OFFSET) = 0;

	while (true) {
		int state = gpio_pin_get_dt(&btn);
		bool isPressed = state == 0;
		
		gpio_pin_set_dt(&neopixel, 0);
		DELAY_NS(NEO_RESET_NS);

		//for (int j = 0; j < NEO_NUM_LEDS; j++) {
			//uint32_t* sharedRgbBase = (uint32_t*)((uint8_t*)pSharedMem + SHARED_RGB_OFFSET);
			for (int i = 0; i < NEO_NUM_LEDS; i++) {
                write_led_color(i, 0x00000000);
            }
			uint32_t color = 0x00000000;
			for (int i = 31; i >= 0; i--) {
				if (color & ((uint32_t)1 << i)) {
					gpio_pin_set_dt(&neopixel, 1);
					NEO_DELAY_ONE_ON();
					gpio_pin_set_dt(&neopixel, 0);
					NEO_DELAY_ONE_OFF();
				} else {
					gpio_pin_set_dt(&neopixel, 1);
					NEO_DELAY_ZERO_ON();
					gpio_pin_set_dt(&neopixel, 0);
					NEO_DELAY_ZERO_OFF();
				}
			}
		//}
		
		MEM_UINT32(IS_BUTTON_PRESSED_OFFSET) = isPressed;
		gpio_pin_set_dt(&neopixel, 0);
		NEO_DELAY_RESET();
	}
	return 0;
}
