#pragma once

#include <thread>
#include <mutex>
#include <alsa/asoundlib.h>
#include <limits.h>
#include <atomic>

// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.

struct WaveData {
    int numSamples;
    short* data;
};

struct PlaybackSound {
    WaveData* sound;
    int location;
};

class AudioMixer {
public:
    AudioMixer();
    ~AudioMixer();

    // Queue up another sound bite to play as soon as possible.
    void queueSound(WaveData* sound);

    // Read the contents of a wave file into the pSound structure. Note that
    // the pData pointer in this structure will be dynamically allocated in
    // readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
    void readWaveFileIntoMemory(const std::string fileName, WaveData* sound);
    void freeWaveFileData(WaveData* sound);

    // Get/set the volume.
    // setVolume() function posted by StackOverflow user "trenki" at:
    // http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
    void setVolume(int newVolume);
    int getVolume();

private:
    static constexpr int DEFAULT_VOLUME = 80;
    static constexpr int MAX_SOUND_BITES = 30;
    static constexpr int NUM_CHANNELS = 1;
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int FILL_BUFFER_WAIT = 17;
    static constexpr size_t SAMPLE_SIZE = sizeof(short);
    static constexpr int MIN_VOLUME = 0;
    static constexpr int MAX_VOLUME = 100;

    std::atomic<bool> isRunning;
    std::atomic<int> volume;
    short* playbackBuffer;
    unsigned long playbackBufferSize;
    std::thread playbackThread;
    std::mutex playbackMutex;
    snd_pcm_t* handle;
    PlaybackSound soundBites[MAX_SOUND_BITES];

    void process();
    void fillPlaybackBuffer(short* buff, int size);

    
};