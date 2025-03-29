#pragma once

#include <fstream>
#include <string>
#include <unistd.h>

class Microservo {
public:
    Microservo(const std::string& filePath, const int& period, const int& minAngle, const int& maxAngle);
    ~Microservo();

    void setServoAngle(const int& angle);
    void increaseAngle(const int& angle);
    void decreaseAngle(const int& angle);

private:
    std::string filePath;
    int minAngle;
    int maxAngle;
    int curAngle;

    void writeToFile(const std::string& filePath, const std::string& file, const std::string& value);
    void setEnable(const int& value);
    void setDutyCycle(const int& value);
    void setPeriod(const int& hertz);
};