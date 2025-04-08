// /*
//  * R5 Sample Code for Shared Memory with Linux
//  */

//  #include <stdio.h>
//  #include <stdlib.h>
//  #include <zephyr/kernel.h>
//  #include <zephyr/drivers/gpio.h>
//  #include <string.h>
 
//  #include "sharedDataLayout.h"
//  #include "neopixel.h"
//  #include "sharedMem.h"
 
//  static void *pSharedMem = (void *) SharedMem::BTCM_START;
 
//  static uint32_t horizontalColor = 0x0;
 
//  // Device tree nodes for pin aliases
//  #define LED0_NODE DT_ALIAS(led0)
//  // #define BTN0_NODE DT_ALIAS(btn0)
//  #define NEOPIXEL_NODE DT_ALIAS(neopixel)
 
//  static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
//  // static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
//  static const struct gpio_dt_spec neopixel = GPIO_DT_SPEC_GET(NEOPIXEL_NODE, gpios);
 
//  static void initialize_gpio(const gpio_dt_spec* pin, int direction) 
//  {
// 	 if (!gpio_is_ready_dt(pin)) {
// 		 printf("ERROR: GPIO pin not ready read; direction %d\n", direction);
// 		 exit(EXIT_FAILURE);
// 	 }
 
// 	 int ret = gpio_pin_configure_dt(pin, direction);
// 	 if (ret < 0) {
// 		 printf("ERROR: GPIO Pin Configure issue; direction %d\n", direction);
// 		 exit(EXIT_FAILURE);
// 	 }
//  }
 
//  void send_color(uint32_t colour) {
// 	 for(int i = 31; i >= 0; i--) {
// 		 gpio_pin_set_dt(&neopixel, 1);
// 		 if(colour & ((uint32_t)0x1 << i)) {
// 			 NeoPixel::delayZeroOn();
// 			 gpio_pin_set_dt(&neopixel, 0);
// 			 NeoPixel::delayZeroOff();
// 		 } else {
// 			 NeoPixel::delayZeroOn();
// 			 gpio_pin_set_dt(&neopixel, 0);
// 			 NeoPixel::delayZeroOff();
// 		 }
// 	 }
//  }
 
//  void update_leds(uint32_t color) {
// 	 gpio_pin_set_dt(&neopixel, 0);
// 	 NeoPixel::delayNs(NeoPixel::RESET_NS);
	 
// 	 for(int j = 0; j < NeoPixel::NUM_LEDS; j++) {
// 		 send_color(color);
// 	 }
	 
// 	 gpio_pin_set_dt(&neopixel, 0);
// 	 NeoPixel::delayReset();
//  }
  
//  int main() {
// 	 initialize_gpio(&led, GPIO_OUTPUT_ACTIVE);
// 	 initialize_gpio(&neopixel, GPIO_OUTPUT_ACTIVE);
	 
// 	 // Setup defaults
// 	 strcpy((char*) SharedDataLayout::MSG_OFFSET, "Checking if shared memory works.");
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_DELAY_MS_OFFSET) = SharedMem::DEFAULT_LED_DELAY_MS;
// 	 // Just do random values don't bombard eyes with bright lights
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_1_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_2_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_3_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_4_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_5_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_6_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_7_OFFSET) = 0;
// 	 SharedDataLayout::mem_uint32(SharedDataLayout::LED_8_OFFSET) = 0;
 
// 	 //TODO: Depending on offset values, change LED color and position
// 	 while (true) {
// 		 horizontalColor = SharedDataLayout::mem_uint32(SharedDataLayout::HORIZONTAL_DIR_OFFSET);
 
// 		 // Continuously update LEDs based on offset values
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_1_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_2_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_3_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_4_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_5_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_6_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_7_OFFSET));
// 		 send_color(SharedDataLayout::mem_uint32(SharedDataLayout::LED_8_OFFSET));
 
// 		 uint32_t delay = SharedDataLayout::mem_uint32(SharedDataLayout::LED_DELAY_MS_OFFSET);
// 		 //  printf("Waiting for %d ms\n", delay);
// 		 k_busy_wait(delay * SharedMem::MICROSECONDS_PER_MILLISECOND);	
// 	 }
// 	 return 0;
//  }