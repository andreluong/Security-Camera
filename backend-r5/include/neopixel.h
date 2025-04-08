#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <cstdint>

namespace NeoPixel {

    constexpr int NUM_LEDS = 8;

    // NeoPixel Timing
    // NEO_<one/zero>_<on/off>_NS
    // (These times are what the hardware needs; the delays below are hand-tuned to give these).
    constexpr int ONE_ON_NS     = 700;
    constexpr int ONE_OFF_NS    = 600;
    constexpr int ZERO_ON_NS    = 350;
    constexpr int ZERO_OFF_NS	= 800;
    constexpr int RESET_NS		= 60000; // Must be at least 50us, use 60us

    // Delay time includes 1 GPIO set action.
    inline volatile int junk_delay = 0;
    inline void delay350ns() {}
    inline void delay600ns() { for (junk_delay = 0; junk_delay < 9;  junk_delay++); }
    inline void delay700ns() { for (junk_delay = 0; junk_delay < 16; junk_delay++); }
    inline void delay800ns() { for (junk_delay = 0; junk_delay < 23; junk_delay++); }

    inline void delayNs(int ns) {
        int target = k_cycle_get_32() + k_ns_to_cyc_near32(ns);
        while (k_cycle_get_32() < target);
    }
    inline void delayOneOn()   { delay700ns(); }
    inline void delayOneOff()  { delay600ns(); }
    inline void delayZeroOn()  { delay350ns(); }
    inline void delayZeroOff() { delay800ns(); }
    inline void delayReset()   { delayNs(RESET_NS); }

    // Colours
    constexpr uint32_t GREEN              = 0x0f000000;
    constexpr uint32_t RED                = 0x000f0000;
    constexpr uint32_t BLUE               = 0x00000f00;
    constexpr uint32_t WHITE              = 0x0000000f;
    constexpr uint32_t WHITE_RGB          = 0x0f0f0f00;
    constexpr uint32_t YELLOW             = 0x0f0f0000;
    constexpr uint32_t PURPLE             = 0x000f0f00;
    constexpr uint32_t TEAL               = 0x0f000f00;

    constexpr uint32_t GREEN_BRIGHT       = 0xff000000;
    constexpr uint32_t RED_BRIGHT         = 0x00ff0000;
    constexpr uint32_t BLUE_BRIGHT        = 0x0000ff00;
    constexpr uint32_t WHITE_BRIGHT       = 0xffffff00;

    constexpr uint32_t GREEN_WHITE_BRIGHT = 0xff0000ff;
    constexpr uint32_t RED_WHITE_BRIGHT   = 0x00ff00ff;
    constexpr uint32_t BLUE_WHITE_BRIGHT  = 0x0000ffff;
    constexpr uint32_t WHITE_WHITE_BRIGHT = 0xffffffff;

    constexpr uint32_t BLACK              = 0x00000000;

    // Brighten a color
    inline uint32_t brightenColor(uint32_t color) {
        switch (color) {
            case GREEN: return GREEN_BRIGHT;
            case RED:   return RED_BRIGHT;
            case BLUE:  return BLUE_BRIGHT;
            default:    return BLACK;
        }
    }

}	

#endif