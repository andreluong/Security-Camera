#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

// R5 Shared Memory Note
// - It seems that using a struct for the ATCM memory does not work 
//   (hangs when accessing memory via a struct pointer).
// - Therefore, using an array.

#define MSG_OFFSET 0x7000
#define MSG_SIZE   32
#define LIGHT_SIZE 8
#define LIGHT_VALUES_SIZE (LIGHT_SIZE * sizeof(uint32_t))

#define LED_DELAY_MS_OFFSET (MSG_OFFSET + MSG_SIZE)
#define HORIZONTAL_DIR_OFFSET (LED_DELAY_MS_OFFSET + sizeof(uint32_t)) // Switch LED color (based on enum maybe?)
#define UP_DOWN_OFFSET (HORIZONTAL_DIR_OFFSET + sizeof(uint32_t)) // An enum of 8 values? or prob anothe type
#define IS_BUTTON_PRESSED_OFFSET (UP_DOWN_OFFSET + sizeof(uint32_t))
#define LED_1_OFFSET (IS_BUTTON_PRESSED_OFFSET + sizeof(uint32_t))
#define LED_2_OFFSET (LED_1_OFFSET + sizeof(uint32_t))
#define LED_3_OFFSET (LED_2_OFFSET + sizeof(uint32_t))
#define LED_4_OFFSET (LED_3_OFFSET + sizeof(uint32_t))
#define LED_5_OFFSET (LED_4_OFFSET + sizeof(uint32_t))
#define LED_6_OFFSET (LED_5_OFFSET + sizeof(uint32_t))
#define LED_7_OFFSET (LED_6_OFFSET + sizeof(uint32_t))
#define LED_8_OFFSET (LED_7_OFFSET + sizeof(uint32_t))
#define END_MEMORY_OFFSET (LED_8_OFFSET + sizeof(uint32_t))

#define MEM_UINT8(addr) *(uint8_t*)(addr)
#define MEM_UINT32(addr) *(uint32_t*)(addr)
#endif
