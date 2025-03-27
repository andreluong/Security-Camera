#include "joystick.h"
#include <cstdio>

Joystick::Joystick() : is_initialized(false), i2c_file_desc(0) {
    init();
}

Joystick::~Joystick() {
    cleanup();
}

void Joystick::sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, nullptr);
}

void Joystick::init() {
    i2c_file_desc = open(I2C_BUS, O_RDWR);
    if (i2c_file_desc == -1) {
        perror("Unable to open I2C bus");
        exit(EXIT_FAILURE);
    }
    if (ioctl(i2c_file_desc, I2C_SLAVE, I2C_DEVICE_ADDRESS) == -1) {
        perror("Unable to set I2C device to slave address");
        exit(EXIT_FAILURE);
    }
    is_initialized = true;
}

void Joystick::cleanup() {
    assert(is_initialized);
    close(i2c_file_desc);
    is_initialized = false;
}

void Joystick::writeReg(uint8_t reg_addr, uint16_t value) {
    uint8_t buffer[3];
    buffer[0] = reg_addr;
    buffer[1] = static_cast<uint8_t>(value & 0xFF);
    buffer[2] = static_cast<uint8_t>((value & 0xFF00) >> 8);
    if (write(i2c_file_desc, buffer, 3) != 3) {
        perror("Unable to write I2C register");
        exit(EXIT_FAILURE);
    }
}

uint16_t Joystick::readReg(uint8_t reg_addr) {
    if (write(i2c_file_desc, &reg_addr, sizeof(reg_addr)) != sizeof(reg_addr)) {
        perror("Unable to write I2C register.");
        exit(EXIT_FAILURE);
    }
    uint16_t value = 0;
    if (read(i2c_file_desc, &value, sizeof(value)) != sizeof(value)) {
        perror("Unable to read I2C register");
        exit(EXIT_FAILURE);
    }
    return value;
}

uint16_t Joystick::swapAndScale(uint16_t value) {
    uint16_t swap = ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
    return swap >> 4;
}

int Joystick::getX() {
    assert(is_initialized);
    writeReg(REG_CONFIGURATION, X_CHANNEL);
    sleepForMs(5);
    uint16_t raw_read = readReg(REG_DATA);
    uint16_t scaled = swapAndScale(raw_read);

    if (scaled >= maxThresh) return coordMax;
    if (scaled <= minThresh) return coordMin;
    return coordIdle;
}

int Joystick::getY() {
    assert(is_initialized);
    writeReg(REG_CONFIGURATION, Y_CHANNEL);
    sleepForMs(5);
    uint16_t raw_read = readReg(REG_DATA);
    uint16_t scaled = swapAndScale(raw_read);

    if (scaled >= maxThresh) return coordMin;
    if (scaled <= minThresh) return coordMax;
    return coordIdle;
}

JoystickDirection Joystick::getDirection() {
    int xVal = getX();
    if (xVal == coordMax) return JoystickDirection::RIGHT;
    if (xVal == coordMin) return JoystickDirection::LEFT;

    int yVal = getY();
    if (yVal == coordMax) return JoystickDirection::UP;
    if (yVal == coordMin) return JoystickDirection::DOWN;

    return JoystickDirection::IDLE;
}

JoystickDirection Joystick::getXDirection() {
    int xVal = getX();
    if (xVal == coordMax) return JoystickDirection::RIGHT;
    if (xVal == coordMin) return JoystickDirection::LEFT;
    return JoystickDirection::IDLE;
}

JoystickDirection Joystick::getYDirection() {
    int yVal = getY();
    if (yVal == coordMax) return JoystickDirection::UP;
    if (yVal == coordMin) return JoystickDirection::DOWN;
    return JoystickDirection::IDLE;
}
