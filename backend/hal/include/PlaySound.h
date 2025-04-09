#ifndef PLAYSOUND_H
#define PLAYSOUND_H

#include <cstddef>  // for size_t
#include <thread>
#include <atomic>

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
private:
    static std::thread playbackThread;
    static std::atomic<bool> stopping;
};


class Alarm {
public:
    Alarm(const char* SoundPath = "sounds/alarm_guillaume.wav");            
    ~Alarm();           
    void playAlarm();     

private:
    wavedata_t sound;
};

#endif // PLAYSOUND_H