// HAL light sensor module
// To be used for night light feature

#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#define SENSOR_BUFFER 1000
#define WEIGHT_ALPHA 0.01

#include "i2cHelpers.h"
#include <vector>
#include <mutex>


class LightSensor {
    private:
        int i2c_file_desc;
        int numSamplesTaken;
        int sampleIdx;
        double previousAverage;
        double currentAverage;
        bool isRunning;

        std::mutex sensorMutex;
        std::vector<double> samples;
        std::vector<double> history;

        void calculateSimpleMovingAvg() {
            if(numSamplesTaken == 0) {
                previousAverage = samples[0];
            } else {
                previousAverage = (WEIGHT_ALPHA * samples[sampleIdx]) + ((1 - WEIGHT_ALPHA) * previousAverage);
            }
        }

    public:
        LightSensor();
        ~LightSensor();
        void getSamples();
        std::vector<double> getHistory();
        void moveCurrentDataToHistory();
        double getAverageReading();

};


#endif