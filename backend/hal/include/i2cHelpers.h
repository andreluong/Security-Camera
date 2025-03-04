// Module for I2C functionality

#if !defined(_I2C_HELPERS_H_)
#define _I2C_HELPERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdbool.h>

// Device bus & address
#define I2CDRV_LINUX_BUS "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x48

// Register in TLA2024
#define REG_CONFIGURATION 0x01
#define REG_DATA 0x00

// Configuration reg contents for continuously sampling different channels
#define TLA2024_CHANNEL_CONF_0 0x83C2
#define TLA2024_CHANNEL_CONF_1 0x83D2
#define TLA2024_CHANNEL_CONF_2 0x83E2
#define TLA2024_CHANNEL_CONF_3 0x83F2

// ADC channel selectors
#define JOYSTICK_X TLA2024_CHANNEL_CONF_0
#define JOYSTICK_Y TLA2024_CHANNEL_CONF_1
#define LIGHT_SENSOR TLA2024_CHANNEL_CONF_2

#define BUFFER_SIZE ((sizeof(uint16_t)) + (1))

namespace i2cOperations {
    int init_i2c_bus(const char* bus, int address);
    void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value);
    uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr);
    uint16_t swap_and_scale(uint16_t value);
};


#endif // _I2C_HELPERS_H_
