#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <atomic>
#include <thread>

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

private:
    std::atomic<bool> is_initialized;
    std::atomic<bool> is_running;
    int i2c_file_desc;
    std::thread joystickThread;

    void processDirection();
    JoystickDirection getDirection();
    int getX();
    int getY();

    // Constants
    static constexpr int minThresh = 400;
    static constexpr int maxThresh = 1100;
    static constexpr int coordIdle = 0;
    static constexpr int coordMin = -100;
    static constexpr int coordMax = 100;
};

#endif // JOYSTICK_H