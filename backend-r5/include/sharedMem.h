#ifndef SHARED_MEM_H
#define SHARED_MEM_H

 // Memory
 // ----------------------------------------
 #define SHARED_MEM_BTCM_START 0x00000000  // TRM p848
 #define SHARED_MEM_ATCM_START 0x00041010  // TRM p849
 
 // Access GPIO (for demonstration purposes)
 // ----------------------------------------
 // 1,000,000 uSec = 1000 msec = 1 sec
 #define MICRO_SECONDS_PER_MILI_SECOND   (1000)
 #define DEFAULT_LED_DELAY_MS            (100)

#endif