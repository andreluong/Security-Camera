#include "Alarm.h"
#include <cstdio>

#include <AudioMixer.h>

Alarm::Alarm(AudioMixer& mixer, const char* soundPath) : audioMixer(mixer) {
    // audioMixer = std::make_unique<AudioMixer>();
    // audioMixer->readWaveFileIntoMemory(soundPath, &sound);
    audioMixer.readWaveFileIntoMemory(soundPath, &sound);
}

Alarm::~Alarm() {
    // audioMixer->freeWaveFileData(&sound);
    audioMixer.freeWaveFileData(&sound);
    std::printf("Alarm module shutdown.\n");
}

void Alarm::alert() {
    audioMixer.queueSound(&sound);
    // audioMixer->queueSound(&sound);
}

// Alarm::Alarm(const char* soundPath) {
//     Audio::init();
//     Audio::readWaveFileIntoMemory(soundPath, &sound);
// }

// Alarm::~Alarm() {
//     Audio::freeWaveFileData(&sound);
//     Audio::cleanup();
//     std::printf("Alarm module shutdown.\n");
// }

// void Alarm::alert() {
//     Audio::queueSound(&sound);
// }
