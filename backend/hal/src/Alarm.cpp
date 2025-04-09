
#include "Alarm.h"
#include <cstdio>

#include <AudioMixer.h>

Alarm::Alarm(AudioMixer& mixer, const char* soundPath) : audioMixer(mixer) {
    audioMixer.readWaveFileIntoMemory(soundPath, &sound);
}

Alarm::~Alarm() {
    audioMixer.freeWaveFileData(&sound);
    std::printf("Alarm module shutdown.\n");
}

void Alarm::alert() {
    audioMixer.queueSound(&sound);
}