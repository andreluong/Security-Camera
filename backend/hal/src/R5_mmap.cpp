#include "R5_mmap.h"

// Return the address of the base address of the ATCM memory region for the R5-MCU
volatile void* getR5MmapAddr()
{
    // Access /dev/mem to gain access to physical memory (for memory-mapped devices/specialmemory)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem; Did you run with sudo?");
        exit(EXIT_FAILURE);
    }

    // Inside main memory (fd), access the part at offset BTCM_ADDR:
    // (Get points to start of R5 memory after it's memory mapped)
    volatile void* pR5Base = mmap(0, MEM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BTCM_ADDR);
    if (pR5Base == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pR5Base;
}

void freeR5MmapAddr(volatile void* pR5Base)
{
    if (munmap((void*) pR5Base, MEM_LENGTH)) {
        perror("R5 munmap failed");
        exit(EXIT_FAILURE);
    }
}