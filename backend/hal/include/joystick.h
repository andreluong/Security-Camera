#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


enum class JoystickDirection {
    IDLE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Joystick {
public:
    Joystick();
    ~Joystick();

    void init();
    void cleanup();

    JoystickDirection getDirection();
    JoystickDirection getXDirection();
    JoystickDirection getYDirection();

private:
    bool is_initialized;
    int i2c_file_desc;

    int getX();
    int getY();
    void sleepForMs(long long delayInMs);
    void writeReg(uint8_t reg_addr, uint16_t value);
    uint16_t readReg(uint8_t reg_addr);
    uint16_t swapAndScale(uint16_t value);

    // Constants
    static constexpr const char* I2C_BUS = "/dev/i2c-1";
    static constexpr int I2C_DEVICE_ADDRESS = 0x48;
    static constexpr uint8_t REG_CONFIGURATION = 0x01;
    static constexpr uint8_t REG_DATA = 0x00;
    static constexpr uint16_t X_CHANNEL = 0x83D2;
    static constexpr uint16_t Y_CHANNEL = 0x83C2;

    static constexpr int minThresh = 10;
    static constexpr int maxThresh = 1590;
    static constexpr int coordIdle = 0;
    static constexpr int coordMin = -100;
    static constexpr int coordMax = 100;
};

#endif // JOYSTICK_H