#ifndef LIGHTSENSOR2_H
#define LIGHTSENSOR2_H

#include <cstdint>

class LightSensor {
public:
    LightSensor();
    ~LightSensor();
    uint16_t readLightLevel();

private:
    int i2cFileDescriptor;
    void writeRegister(uint8_t reg, uint16_t value);
    uint16_t readRegister(uint8_t reg);
    uint16_t processRawData(uint16_t rawData);
};

#endif // LIGHTSENSOR_H