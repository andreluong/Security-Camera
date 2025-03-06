#include "lightSensor.h"

LightSensor::LightSensor() {
    i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    samples.reserve(SENSOR_BUFFER);
    history.reserve(SENSOR_BUFFER);
}

LightSensor::~LightSensor() {
    close(i2c_file_desc);
}