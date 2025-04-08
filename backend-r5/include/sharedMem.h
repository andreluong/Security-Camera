#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <cstdint>

namespace SharedMem {

    // Memory
    // ----------------------------------------
    constexpr std::uintptr_t BTCM_START = 0x00000000;   // TRM p848
    constexpr std::uintptr_t ATCM_START = 0x00041010;   // TRM p849

    // Access GPIO (for demonstration purposes)
    // ----------------------------------------
    // 1,000,000 uSec = 1000 msec = 1 sec
    constexpr std::uint32_t MICROSECONDS_PER_MILLISECOND = 1000;
    constexpr std::uint32_t DEFAULT_LED_DELAY_MS = 100;

}

#endif