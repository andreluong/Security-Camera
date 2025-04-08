#include "light_sensor.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cassert>
#include <cstdio>

#define I2C_BUS "/dev/i2c-1"           // I2C bus device on the BeagleBone Black
#define I2C_ADDRESS 0x48               // I2C address of the TLA2024 ADC
#define CONFIG_REGISTER 0x01           // Configuration register
#define DATA_REGISTER 0x00             // Data register
#define CHANNEL_3_CONFIG 0x83F2        // Config value to read from channel 3

LightSensor::LightSensor() : i2cFileDescriptor(-1), isInitialized(false) {
    initialize();
}

LightSensor::~LightSensor() {
    cleanup();
}

void LightSensor::initialize() {
    i2cFileDescriptor = open(I2C_BUS, O_RDWR);
    if (i2cFileDescriptor < 0) {
        perror("Failed to open I2C bus");
        exit(EXIT_FAILURE);
    }
    if (ioctl(i2cFileDescriptor, I2C_SLAVE, I2C_ADDRESS) < 0) {
        perror("Failed to set I2C slave address");
        exit(EXIT_FAILURE);
    }
    isInitialized = true;
}

void LightSensor::cleanup() {
    if (isInitialized) {
        close(i2cFileDescriptor);
        isInitialized = false;
    }
}

void LightSensor::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t buffer[3];
    buffer[0] = reg;
    buffer[1] = static_cast<uint8_t>(value & 0xFF);
    buffer[2] = static_cast<uint8_t>((value & 0xFF00) >> 8);
    if (write(i2cFileDescriptor, buffer, 3) != 3) {
        perror("Unable to write I2C register");
        exit(EXIT_FAILURE);
    }
}

uint16_t LightSensor::readRegister(uint8_t reg) {
    if (write(i2cFileDescriptor, &reg, sizeof(reg)) != sizeof(reg)) {
        perror("Unable to write I2C register.");
        exit(EXIT_FAILURE);
    }
    uint16_t value = 0;
    if (read(i2cFileDescriptor, &value, sizeof(value)) != sizeof(value)) {
        perror("Unable to read I2C register");
        exit(EXIT_FAILURE);
    }
    return value;
}

uint16_t LightSensor::processRawData(uint16_t rawData) {
    uint16_t swapped = ((rawData & 0xFF00) >> 8) | ((rawData & 0x00FF) << 8);
    return swapped >> 4; // Convert to 12-bit value
}

uint16_t LightSensor::readLightLevel() {
    assert(isInitialized);
    writeRegister(CONFIG_REGISTER, CHANNEL_3_CONFIG);
    usleep(1000); // Wait for conversion to complete
    uint16_t rawData = readRegister(DATA_REGISTER);
    return processRawData(rawData);
}