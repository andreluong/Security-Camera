#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <cstdint>

class LightSensor {
public:
    LightSensor();
    ~LightSensor();
    uint16_t readLightLevel();
private:
    int i2cFileDescriptor;
};

#endif // LIGHTSENSOR_H