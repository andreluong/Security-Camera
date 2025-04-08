#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <cstdint>

// R5 Shared Memory Note:
// Using a struct for ATCM memory access caused issues (hangs), so use raw memory array access.

namespace SharedDataLayout {

    // Constants
    constexpr std::size_t MSG_OFFSET = 0x7000;
    constexpr std::size_t MSG_SIZE = 32;

    constexpr std::size_t LIGHT_SIZE = 8;
    constexpr std::size_t LIGHT_VALUES_SIZE = LIGHT_SIZE * sizeof(uint32_t);

    // Offsets (based on memory layout)
    constexpr std::size_t LED_DELAY_MS_OFFSET = MSG_OFFSET + MSG_SIZE;
    constexpr std::size_t HORIZONTAL_DIR_OFFSET = LED_DELAY_MS_OFFSET + sizeof(uint32_t);
    constexpr std::size_t UP_DOWN_OFFSET = HORIZONTAL_DIR_OFFSET + sizeof(uint32_t);
    constexpr std::size_t IS_BUTTON_PRESSED_OFFSET = UP_DOWN_OFFSET + sizeof(uint32_t);

    constexpr std::size_t LED_1_OFFSET = IS_BUTTON_PRESSED_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_2_OFFSET = LED_1_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_3_OFFSET = LED_2_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_4_OFFSET = LED_3_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_5_OFFSET = LED_4_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_6_OFFSET = LED_5_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_7_OFFSET = LED_6_OFFSET + sizeof(uint32_t);
    constexpr std::size_t LED_8_OFFSET = LED_7_OFFSET + sizeof(uint32_t);

    constexpr std::size_t END_MEMORY_OFFSET = LED_8_OFFSET + sizeof(uint32_t);

    // Memory access helpers
    inline uint8_t& mem_uint8(std::uintptr_t addr) {
        return *reinterpret_cast<uint8_t*>(addr);
    }

    inline uint32_t& mem_uint32(std::uintptr_t addr) {
        return *reinterpret_cast<uint32_t*>(addr);
    }

}

#endif