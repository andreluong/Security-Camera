#include "lightSensor.h"
#include "i2cHelpers.h"

#include <iostream>

LightSensor::LightSensor() {
    i2c_file_desc = i2cOperations::init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    numSamplesTaken = 0;
    previousAverage = 0;
    currentAverage = 0;
    sampleIdx = 0;
    isRunning = true;
    samples.reserve(SENSOR_BUFFER);
    history.reserve(SENSOR_BUFFER);
}

LightSensor::~LightSensor() {
    close(i2c_file_desc);
}

void LightSensor::getSamples() {
    while(isRunning) {
        i2cOperations::write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, LIGHT_SENSOR);
        uint16_t raw_read = i2cOperations::read_i2c_reg16(i2c_file_desc, REG_DATA);
        raw_read = i2cOperations::swap_and_scale(raw_read);
        double value = ((double) raw_read / 4096) * 3.3;

        sensorMutex.lock();
        samples.push_back(value);
        numSamplesTaken++;
        sampleIdx = (sampleIdx + 1) % SENSOR_BUFFER;
        calculateSimpleMovingAvg();
        sensorMutex.unlock();

        std::cout << getAverageReading() << std::endl;
    }
}

std::vector<double> LightSensor::getHistory(){
    std::vector<double> historySnapshot;
    sensorMutex.lock();
    std::copy(history.begin(), history.end(), std::back_inserter(historySnapshot));
    sensorMutex.unlock();
    return historySnapshot;
}

void LightSensor::moveCurrentDataToHistory() {
    sensorMutex.lock();
    std::copy(samples.begin(), samples.end(), std::back_inserter(history));
    samples.clear();
    sensorMutex.unlock();
    
    numSamplesTaken = 0;
}

double LightSensor::getAverageReading() {
    currentAverage = previousAverage;
    return currentAverage;
}