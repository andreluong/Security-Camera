#ifndef PLAYSOUND_H
#define PLAYSOUND_H

#include <cstddef>  // for size_t

struct wavedata_t {
    int numSamples;
    short* pData;
};

struct playbackSound_t {
    wavedata_t* pSound;
    int location;
};

class Audio {
public:
    static void init();
    static void cleanup();
    static void queueSound(wavedata_t* sound);
    static void readWaveFileIntoMemory(const char* fileName, wavedata_t* sound);
    static void freeWaveFileData(wavedata_t* sound);
    static void fillPlaybackBuffer(short* buff, int size);
    static void setVolume(int newVolume);
    static int getVolume();
};

// class Sound {
// public:
//     Sound();
//     ~Sound(); 
//     void playSound();

// private:
//     wavedata_t sound;
// };

#endif // PLAYSOUND_H