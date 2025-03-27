#include "PlaySound.h"
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>
#include <unistd.h>

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short))
#define MAX_SOUND_BITES 100
#define AUDIOMIXER_MAX_VOLUME 100

// Internal AudioMixer state
namespace {
    struct playbackSound_t {
        wavedata_t* pSound;
        int location;
    };

    snd_pcm_t* handle = nullptr;
    short* playbackBuffer = nullptr;
    unsigned long playbackBufferSize = 0;
    pthread_t playbackThreadId;
    std::atomic<bool> stopping = false;
    std::mutex audioMutex;
    int volume = 80;
    playbackSound_t soundBites[MAX_SOUND_BITES];
}


void Audio::init() {
    setVolume(80);

    std::lock_guard<std::mutex> lock(audioMutex);
    for (auto& sb : soundBites) {
        sb.pSound = nullptr;
        sb.location = 0;
    }

    int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::cerr << "Playback open error: " << snd_strerror(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                             NUM_CHANNELS, SAMPLE_RATE, 1, 50000);
    if (err < 0) {
        std::cerr << "Playback set_params error: " << snd_strerror(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned long unusedBufferSize = 0;
    snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
    playbackBuffer = new short[playbackBufferSize];

    pthread_create(&playbackThreadId, nullptr, [](void*) -> void* {
        while (!stopping) {
            Audio::fillPlaybackBuffer(playbackBuffer, playbackBufferSize);
            snd_pcm_sframes_t frames = snd_pcm_writei(handle, playbackBuffer, playbackBufferSize);
            if (frames < 0) {
                frames = snd_pcm_recover(handle, frames, 1);
            }
            if (frames < 0) {
                std::cerr << "ERROR: Failed writing audio." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return nullptr;
    }, nullptr);
}

void Audio::cleanup() {
    std::cout << "Stopping audio...\n";
    stopping = true;
    pthread_join(playbackThreadId, nullptr);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    delete[] playbackBuffer;
    playbackBuffer = nullptr;
    std::cout << "Done stopping audio...\n";
}

void Audio::readWaveFileIntoMemory(const char* fileName, wavedata_t* sound) {
    assert(sound);

    const int PCM_DATA_OFFSET = 44;
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        std::cerr << "ERROR: Unable to open file " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
    sound->numSamples = sizeInBytes / SAMPLE_SIZE;

    fseek(file, PCM_DATA_OFFSET, SEEK_SET);
    sound->pData = (short*)malloc(sizeInBytes);
    if (!sound->pData) {
        std::cerr << "ERROR: Unable to allocate memory for " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    int samplesRead = fread(sound->pData, SAMPLE_SIZE, sound->numSamples, file);
    if (samplesRead != sound->numSamples) {
        std::cerr << "ERROR: Mismatch reading samples in " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void Audio::freeWaveFileData(wavedata_t* sound) {
    if (!sound) return;
    free(sound->pData);
    sound->pData = nullptr;
    sound->numSamples = 0;
}

void Audio::queueSound(wavedata_t* sound) {
    assert(sound && sound->numSamples > 0 && sound->pData);

    std::lock_guard<std::mutex> lock(audioMutex);
    for (auto& sb : soundBites) {
        if (!sb.pSound) {
            sb.pSound = sound;
            sb.location = 0;
            return;
        }
    }

    std::cerr << "No available sound bite slots!\n";
}

void Audio::setVolume(int newVolume) {
    if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
        std::cerr << "Volume must be between 0 and 100." << std::endl;
        return;
    }

    volume = newVolume;

    long min, max;
    snd_mixer_t* mixerHandle;
    snd_mixer_selem_id_t* sid;

    const char* card = "default";
    const char* selem_name = "PCM";

    snd_mixer_open(&mixerHandle, 0);
    snd_mixer_attach(mixerHandle, card);
    snd_mixer_selem_register(mixerHandle, NULL, NULL);
    snd_mixer_load(mixerHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);

    snd_mixer_elem_t* elem = snd_mixer_find_selem(mixerHandle, sid);
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
    snd_mixer_close(mixerHandle);
}

int Audio::getVolume() {
    return volume;
}

void Audio::fillPlaybackBuffer(short* buff, int size) {
    memset(buff, 0, size * sizeof(short));

    std::lock_guard<std::mutex> lock(audioMutex);
    for (auto& sb : soundBites) {
        if (!sb.pSound) continue;

        for (int j = 0; j < size; ++j) {
            if (sb.location < sb.pSound->numSamples) {
                int mixed = buff[j] + sb.pSound->pData[sb.location++];
                if (mixed > SHRT_MAX) mixed = SHRT_MAX;
                if (mixed < SHRT_MIN) mixed = SHRT_MIN;
                buff[j] = mixed;
            } else {
                sb.pSound = nullptr;
                break;
            }
        }
    }
}

Sound::Sound() {
    const char* SoundPath = "/mnt/remote/myApps/beatbox-wave-files/267560__alienxxx__beep_sequence_01.wav";
    Audio::readWaveFileIntoMemory(SoundPath, &sound);
}

Sound::~Sound() {
    Audio::freeWaveFileData(&sound);
}

void Sound::playSound() {
    Audio::queueSound(&sound);
}
