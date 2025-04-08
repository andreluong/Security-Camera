#ifndef R5_MMAP_H
#define R5_MMAP_H

#include "sharedDataLayout.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

// General R5 Memory Sharing Routine
// ----------------------------------------------------------------
#define ATCM_ADDR     0x79000000  // MCU ATCM (p59 TRM)
#define BTCM_ADDR     0x79020000  // MCU BTCM (p59 TRM)
#define MEM_LENGTH    0x8000

// Return the address of the base address of the ATCM memory region for the R5-MCU
volatile void* getR5MmapAddr();
void freeR5MmapAddr(volatile void* pR5Base);

#endif