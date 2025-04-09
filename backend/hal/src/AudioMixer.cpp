#include "AudioMixer.h"
#include <cstdio>
#include <cassert>

AudioMixer::AudioMixer() : isRunning(true) {
    setVolume(DEFAULT_VOLUME);

    // Initialize the currently active sound-bites being played
    for (auto& sb : soundBites) {
        sb.sound = nullptr;
        sb.location = -1;
    }

    // Open the PCM output
    int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Configure parameters of PCM output
    err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                             NUM_CHANNELS, SAMPLE_RATE, 1, 50000);
    if (err < 0) {
        std::printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
    unsigned long unusedBufferSize = 0;
    snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
    // ..allocate playback buffer:
    playbackBuffer = new short[playbackBufferSize]; // TODO: Consider unique ptr

    playbackThread = std::thread(&AudioMixer::process, this);
}

AudioMixer::~AudioMixer() {
    isRunning = false;
    if (playbackThread.joinable()) playbackThread.join();
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    delete[] playbackBuffer;
    playbackBuffer = nullptr;
    std::printf("AudioMixer module shutdown.\n");
}

void AudioMixer::process() {
    while (isRunning) {
        // Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

        // Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle, playbackBuffer, playbackBufferSize);
        
        // Check for (and handle) possible error conditions on output
        if (frames < 0) {
            std::fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
            frames = snd_pcm_recover(handle, frames, 1);
        }
        if (frames < 0) {
            std::fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
        }
        if (frames > 0 && frames < (long) playbackBufferSize) {  // <- temporarily commented out to avoid compile error
			std::printf("Short write (expected %li, wrote %li)\n", playbackBufferSize, frames);
		}
    }
}

void AudioMixer::readWaveFileIntoMemory(const std::string fileName, WaveData* sound) {
    assert(sound);

    // The PCM data in a wave file starts after the header:
    const int PCM_DATA_OFFSET = 44;

    FILE* file = fopen(fileName.c_str(), "r");
    if (!file) {
        std::fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName.c_str());
        exit(EXIT_FAILURE);
    }

    // Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	sound->numSamples = sizeInBytes / SAMPLE_SIZE;

    // Search to the start of the data in the file
    fseek(file, PCM_DATA_OFFSET, SEEK_SET);

    // Allocate space to hold all PCM data
    sound->data = (short*)malloc(sizeInBytes);
    if (!sound->data) {
        std::fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n", sizeInBytes, fileName.c_str());
        exit(EXIT_FAILURE);
    }

    // Read PCM data from wave file into memory
    int samplesRead = fread(sound->data, SAMPLE_SIZE, sound->numSamples, file);
    if (samplesRead != sound->numSamples) {
        std::fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				    sound->numSamples, fileName.c_str(), samplesRead);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void AudioMixer::freeWaveFileData(WaveData* sound) {
    if (!sound) return;
    free(sound->data);
    sound->data = nullptr;
    sound->numSamples = 0;
}

void AudioMixer::queueSound(WaveData* sound) {
    // Ensure we are only being asked to play "good" sounds:
    assert(sound && sound->numSamples > 0 && sound->data);

    // Insert the sound by searching for an empty sound bite spot
	/*
	 * REVISIT: Implement this:
	 * 1. Since this may be called by other threads, and there is a thread
	 *    processing the soundBites[] array, we must ensure access is threadsafe.
	 * 2. Search through the soundBites[] array looking for a free slot.
	 * 3. If a free slot is found, place the new sound file into that slot.
	 *    Note: You are only copying a pointer, not the entire data of the wave file!
	 * 4. After searching through all slots, if no free slot is found then print
	 *    an error message to the console (and likely just return vs asserting/exiting
	 *    because the application most likely doesn't want to crash just for
	 *    not being able to play another wave file.
	 */
    playbackMutex.lock();

    for (auto& sb : soundBites) {
        if (!sb.sound) {
            sb.sound = sound;
            sb.location = 0;
            playbackMutex.unlock();
            return;
        }
    }

    playbackMutex.unlock();

    std::fprintf(stderr, "Error! No free slots found.\n");
}

int AudioMixer::getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer::setVolume(int newVolume) {
    if (newVolume < MIN_VOLUME || newVolume > MAX_VOLUME) {
        std::fprintf(stderr, "Volume must be between %d and %d.", MIN_VOLUME, MAX_VOLUME);
        return;
    }

    volume = newVolume;

    long min, max;
    snd_mixer_t* mixerHandle;
    snd_mixer_selem_id_t* sid;

    const char* card = "default";
    const char* selem_name = "PCM"; // For ZEN cape

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

// Fill the buff array with new PCM values to output.
//    buff: buffer to fill with new PCM data from sound bites.
//    size: the number of *values* to store into buff
void AudioMixer::fillPlaybackBuffer(short* buff, int size) {
    /*
	 * REVISIT: Implement this
	 * 1. Wipe the buff to all 0's to clear any previous PCM data.
	 *    Hint: use memset(); read the docs about its use of size.
	 * 2. Since this is called from a background thread, and soundBites[] array
	 *    may be used by any other thread, must synchronize this.
	 * 3. Loop through each slot in soundBites[], which are sounds that are either
	 *    waiting to be played, or partially already played:
	 *    - If the sound bite slot is unused, do nothing for this slot.
	 *    - Otherwise "add" this sound bite's data to the play-back buffer
	 *      (other sound bites needing to be played back will also add to the same data).
	 *      * Record that this portion of the sound bite has been played back by incrementing
	 *        the location inside the data where play-back currently is.
	 *      * If you have now played back the entire sample, free the slot in the
	 *        soundBites[] array.
	 *
	 *
	 * Notes on "adding" PCM samples:
	 * - PCM is stored as signed shorts (between SHRT_MIN and SHRT_MAX).
	 * - When adding values, ensure there is not an overflow. Any values which would
	 *   greater than SHRT_MAX should be clipped to SHRT_MAX; likewise for underflow.
	 * - Don't overflow any arrays!
	 * - Efficiency matters here! The compiler may do quite a bit for you, but it doesn't
	 *   hurt to keep it in mind. Here are some tips for efficiency and readability:
	 *   * If, for each pass of the loop which "adds" you need to change a value inside
	 *     a struct inside an array, it may be faster to first load the value into a local
	 *      variable, increment this variable as needed throughout the loop, and then write it
	 *     back into the struct inside the array after. For example:
	 *           int offset = myArray[someIdx].value;
	 *           for (int i =...; i < ...; i++) {
	 *               offset ++;
	 *           }
	 *           myArray[someIdx].value = offset;
	 *   * If you need a value in a number of places, try loading it into a local variable
	 *          int someNum = myArray[someIdx].value;
	 *          if (someNum < X || someNum > Y || someNum != Z) {
	 *              someNum = 42;
	 *          }
	 *          ... use someNum vs myArray[someIdx].value;
	 *
	 */

     // Clear previous PCM data
	memset(buff, 0, sizeof(*buff) * size);

    playbackMutex.lock();
    {
        for (auto& sb : soundBites) {
            if (!sb.sound) continue;

            int numSamples = sb.sound->numSamples;
			int remaining = numSamples - sb.location;
			int count = (remaining < size)
				? remaining
				: size;

            // Add samples
			for (int j = 0; j < count; j++) {
				int value = buff[j] + sb.sound->data[sb.location + j];
				if (value > SHRT_MAX) value = SHRT_MAX;
				if (value < SHRT_MIN) value = SHRT_MIN;

				buff[j] = (short)value;
			}
			sb.location += count;

			// Played entire sample, free slot
			if (numSamples - sb.location <= 0) {
				sb.sound = nullptr;
				sb.location = -1;
			}
        }
    }
    playbackMutex.unlock();
}