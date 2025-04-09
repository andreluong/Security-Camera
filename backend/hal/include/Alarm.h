#pragma once

#include "PlaySound.h"
#include "AudioMixer.h"
#include <memory>

class Alarm {
public:
    Alarm(AudioMixer& mixer, const char* soundPath = "sounds/alarm_guillaume.wav");
    ~Alarm();
    void alert();
private:
    // std::unique_ptr<AudioMixer> audioMixer;
    AudioMixer& audioMixer;
    WaveData sound;
};