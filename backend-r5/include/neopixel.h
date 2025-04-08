#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#define NEO_NUM_LEDS          8   // # LEDs in our string

// NeoPixel Timing
// NEO_<one/zero>_<on/off>_NS
// (These times are what the hardware needs; the delays below are hand-tuned to give these).
#define NEO_ONE_ON_NS       700   // Stay on 700ns
#define NEO_ONE_OFF_NS      600   // (was 800)
#define NEO_ZERO_ON_NS      350
#define NEO_ZERO_OFF_NS     800   // (Was 600)
#define NEO_RESET_NS      60000   // Must be at least 50us, use 60us

// Delay time includes 1 GPIO set action.
volatile int junk_delay = 0;
#define DELAY_350_NS() {}
#define DELAY_600_NS() {for (junk_delay=0; junk_delay<9 ;junk_delay++);}
#define DELAY_700_NS() {for (junk_delay=0; junk_delay<16 ;junk_delay++);}
#define DELAY_800_NS() {for (junk_delay=0; junk_delay<23 ;junk_delay++);}

#define DELAY_NS(ns) do {int target = k_cycle_get_32() + k_ns_to_cyc_near32(ns); \
	while(k_cycle_get_32() < target) ; } while(false)

#define NEO_DELAY_ONE_ON()     DELAY_700_NS()
#define NEO_DELAY_ONE_OFF()    DELAY_600_NS()
#define NEO_DELAY_ZERO_ON()    DELAY_350_NS()
#define NEO_DELAY_ZERO_OFF()   DELAY_800_NS()
#define NEO_DELAY_RESET()      {DELAY_NS(NEO_RESET_NS);}


/** COLORS */
#define GREEN 0x0f000000 // Green
#define RED 0x000f0000 // Red
#define BLUE 0x00000f00 // Blue
#define WHITE 0x0000000f // White
#define WHITE_RGB 0x0f0f0f00 // White (via RGB)
#define YELLOW 0x0f0f0000 // Yellow
#define PURPLE 0x000f0f00 // Purple
#define TEAL 0x0f000f00 // Teal

#define GREEN_BRIGHT 0xff000000 // Green Bright
#define RED_BRIGHT   0x00ff0000 // Red Bright
#define BLUE_BRIGHT  0x0000ff00 // Blue Bright
#define WHITE_BRIGHT 0xffffff00 // White
#define GREEN_WHITE_BRIGHT 0xff0000ff // Green Bright w/ Bright White
#define RED_WHITE_BRIGHT 0x00ff00ff // Red Bright w/ Bright White
#define BLUE_WHITE_BRIGHT 0x0000ffff // Blue Bright w/ Bright White
#define WHITE_WHITE_BRIGHT 0xffffffff // White w/ Bright White

#define BLACK 0x00000000

// Brighten a color 
uint32_t brighten_color(uint32_t color) {
    switch(color) {
		case GREEN:
			return GREEN_BRIGHT;
		case RED:
			return RED_BRIGHT;
		case BLUE:
			return BLUE_BRIGHT;
		default:
			return BLACK;
	}
}

// TODO: Animation functions?


#endif