// HAL light sensor module
// To be used for night light feature

#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#define SENSOR_BUFFER 1000

#include "i2cHelpers.h"
#include <vector>


class LightSensor {
    private:
        int i2c_file_desc;
        std::vector<double> samples;
        std::vector<double> history;
    public:
        LightSensor();
        ~LightSensor();
        double getAverageReading();

};

#endif