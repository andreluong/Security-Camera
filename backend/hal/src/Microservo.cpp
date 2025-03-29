#include "Microservo.h"
#include <iostream>
#include <algorithm>

const std::string& PERIOD = "period";
const std::string& DUTY_CYCLE = "duty_cycle";
const std::string& ENABLE = "enable";
const std::string& WRITE_MODE = "w";

constexpr int ONE_SECOND_MS = 1000000000;
constexpr int MIN_DUTY_CYCLE = 500000; // 0.5ms pulse width
constexpr int MAX_DUTY_CYCLE = 2500000; // 2.5ms pulse width
constexpr int MAX_ROTATION = 180;

Microservo::Microservo(const std::string& filePath, const int& period, const int& minAngle, const int& maxAngle)
    : filePath(filePath), minAngle(minAngle), maxAngle(maxAngle), curAngle(0) {
    setDutyCycle(0);
    setPeriod(period); 
    setEnable(1);

    setServoAngle(curAngle);
}

Microservo::~Microservo() {
    setEnable(0);
}

void Microservo::setServoAngle(const int& angle) {
    curAngle = std::clamp(angle, minAngle, maxAngle);
    int dutyCycle = MIN_DUTY_CYCLE + ((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) * curAngle) / MAX_ROTATION;
    setDutyCycle(dutyCycle);
    sleep(2); // Give time to process
}

void Microservo::increaseAngle(const int& angle) {
    if (angle > 0) {
        setServoAngle(curAngle + angle);
    }
}

void Microservo::decreaseAngle(const int& angle) {
    if (angle > 0) {
        setServoAngle(curAngle - angle);
    }
}

void Microservo::writeToFile(const std::string& filePath, const std::string& file, const std::string& value) {
    std::ofstream out;
    out.open(filePath + file);
    if (!out) {
        std::cerr << "Error! Can't write " << value << " to path: " << filePath << std::endl;
        return;
    }
    out << value;
    out.close();
}

void Microservo::setEnable(const int& value) {
    writeToFile(filePath, ENABLE, std::to_string(value));
}

void Microservo::setDutyCycle(const int& value) {
    writeToFile(filePath, DUTY_CYCLE, std::to_string(value));
}

void Microservo::setPeriod(const int& hertz) {
    auto frequencyNs = ONE_SECOND_MS / hertz;
    writeToFile(filePath, PERIOD, std::to_string(frequencyNs));
}