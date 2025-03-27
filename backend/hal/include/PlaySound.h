#ifndef PLAYSOUND_H
#define PLAYSOUND_H

#include <cstddef>  // for size_t

// Structure for holding WAV audio data
struct wavedata_t {
    int numSamples;
    short* pData;
};

// AudioMixer class for low-level audio playback
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

// BeatBox class for managing and playing a single snare sound
class Sound {
public:
    Sound();            // Loads snare audio on construction
    ~Sound();           // Frees snare memory on destruction
    void playSound();     // Queue snare sound for playback

private:
    wavedata_t sound;
};

#endif // PLAYSOUND_H