#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <iostream>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define FRAME_SIZE (CHANNELS * sizeof(int16_t))
#define BUFFER_FRAMES 512

const float kGain = 5.0f; // Gain multiplier

void apply_gain(int16_t *buffer, size_t samples, float gain) {
    for (size_t i = 0; i < samples; ++i) {
        int32_t sample = buffer[i] * gain;
        if (sample > INT16_MAX) sample = INT16_MAX;
        if (sample < INT16_MIN) sample = INT16_MIN;
        buffer[i] = (int16_t)sample;
    }
}

int main() {
    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    int err;

    // Open I2S capture device
    if ((err = snd_pcm_open(&capture_handle, "hw:3,1", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "Cannot open input audio device: %s\n", snd_strerror(err));
        return 1;
    }

    // Open default playback device
    if ((err = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "Cannot open output audio device: %s\n", snd_strerror(err));
        return 1;
    }

    // Configure capture device
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_period_size(capture_handle, hw_params, BUFFER_FRAMES, 0);
    snd_pcm_hw_params(capture_handle, hw_params);

    // Configure playback device
    snd_pcm_hw_params_any(playback_handle, hw_params);
    snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(playback_handle, hw_params, SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_period_size(playback_handle, hw_params, BUFFER_FRAMES, 0);
    snd_pcm_hw_params(playback_handle, hw_params);

    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(capture_handle);
    snd_pcm_prepare(playback_handle);
    
    int16_t buffer[BUFFER_FRAMES * CHANNELS];

    // Start audio loop
    while (1) {
        memset(buffer, 0, sizeof(buffer));        
        err = snd_pcm_readi(capture_handle, buffer, BUFFER_FRAMES);
        if (err < 0) {
            fprintf(stderr, "Read error: %s\n", snd_strerror(err));
            snd_pcm_prepare(capture_handle);
            continue;
        }

        apply_gain(buffer, BUFFER_FRAMES * CHANNELS, kGain);
        std::cout << buffer[0] << std::endl;

        err = snd_pcm_writei(playback_handle, buffer, BUFFER_FRAMES);
        if (err < 0) {
            fprintf(stderr, "Write error: %s\n", snd_strerror(err));
            snd_pcm_prepare(playback_handle);
        }
    }

    snd_pcm_close(capture_handle);
    snd_pcm_close(playback_handle);
    return 0;
}
