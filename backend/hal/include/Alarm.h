#pragma once

#include "AudioMixer.h"
#include <memory>

class Alarm {
public:
    Alarm(AudioMixer& mixer, const char* soundPath = "sounds/alarm_guillaume.wav");
    ~Alarm();
    void alert();
private:
    AudioMixer& audioMixer;
    WaveData sound;
};