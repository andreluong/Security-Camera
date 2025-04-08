#include "LightSensor.h"
#include <iostream>
#include <cstdio>
#include "i2cHelpers.h"
#include <thread>
#include <chrono>

LightSensor::LightSensor() {
    i2cFileDescriptor = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
}

LightSensor::~LightSensor() {
    close(i2cFileDescriptor);
    printf("Light Sensor module shutdown.\n");
}

uint16_t LightSensor::readLightLevel() {
    i2cOperations::write_i2c_reg16(i2cFileDescriptor, REG_CONFIGURATION, LIGHT_SENSOR);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto rawRead = i2cOperations::read_i2c_reg16(i2cFileDescriptor, REG_DATA);
    auto scaledRead = i2cOperations::swap_and_scale(rawRead);
    return scaledRead;
}