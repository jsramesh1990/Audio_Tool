#include "audio_engine.h"
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

static snd_pcm_t* pcm_handle = NULL;
static AudioConfig current_config = {0};
static char last_error[256] = {0};
static bool is_initialized = false;
static pthread_t playback_thread;
static volatile bool playback_running = false;

// Internal functions
static void set_error(const char* msg) {
    strncpy(last_error, msg, sizeof(last_error) - 1);
    last_error[sizeof(last_error) - 1] = '\0';
}

const char* audio_get_error(void) {
    return last_error;
}

void audio_clear_error(void) {
    last_error[0] = '\0';
}

// Main initialization
int audio_init(AudioConfig* config) {
    int err;
    snd_pcm_hw_params_t* hw_params;
    
    if (!config) {
        set_error("NULL config pointer");
        return -1;
    }
    
    // Use default device if not specified
    if (config->device_name[0] == '\0') {
        strcpy(config->device_name, "default");
    }
    
    // Open PCM device
    err = snd_pcm_open(&pcm_handle, config->device_name, 
                       SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        snprintf(last_error, sizeof(last_error), 
                 "Cannot open audio device %s: %s", 
                 config->device_name, snd_strerror(err));
        return err;
    }
    
    // Allocate hardware parameters
    snd_pcm_hw_params_malloc(&hw_params);
    
    // Initialize with full configuration space
    err = snd_pcm_hw_params_any(pcm_handle, hw_params);
    if (err < 0) {
        set_error("Cannot initialize hardware parameters");
        goto cleanup;
    }
    
    // Set access type
    err = snd_pcm_hw_params_set_access(pcm_handle, hw_params,
                                        SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        set_error("Cannot set access type");
        goto cleanup;
    }
    
    // Set sample format
    snd_pcm_format_t format;
    switch(config->bits_per_sample) {
        case 16: format = SND_PCM_FORMAT_S16_LE; break;
        case 24: format = SND_PCM_FORMAT_S24_LE; break;
        case 32: format = SND_PCM_FORMAT_S32_LE; break;
        default: format = SND_PCM_FORMAT_S16_LE;
    }
    
    err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, format);
    if (err < 0) {
        set_error("Cannot set sample format");
        goto cleanup;
    }
    
    // Set channels
    err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, 
                                         config->channels);
    if (err < 0) {
        set_error("Cannot set channel count");
        goto cleanup;
    }
    
    // Set sample rate
    uint32_t rate = config->sample_rate;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, 
                                          &rate, NULL);
    if (err < 0) {
        set_error("Cannot set sample rate");
        goto cleanup;
    }
    config->sample_rate = rate;
    
    // Set buffer size
    err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hw_params,
                                                  &config->buffer_size);
    if (err < 0) {
        set_error("Cannot set buffer size");
        goto cleanup;
    }
    
    // Apply hardware parameters
    err = snd_pcm_hw_params(pcm_handle, hw_params);
    if (err < 0) {
        set_error("Cannot apply hardware parameters");
        goto cleanup;
    }
    
    // Prepare device
    err = snd_pcm_prepare(pcm_handle);
    if (err < 0) {
        set_error("Cannot prepare device");
        goto cleanup;
    }
    
    // Save configuration
    memcpy(&current_config, config, sizeof(AudioConfig));
    is_initialized = true;
    
    snd_pcm_hw_params_free(hw_params);
    return 0;
    
cleanup:
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_close(pcm_handle);
    pcm_handle = NULL;
    return err;
}

// Playback thread function
static void* playback_thread_func(void* arg) {
    AudioBuffer* buffer = (AudioBuffer*)arg;
    ssize_t frames_written;
    snd_pcm_sframes_t delay;
    
    while (playback_running) {
        // Write data to PCM device
        frames_written = snd_pcm_writei(pcm_handle, 
                                        buffer->buffer,
                                        buffer->data_size / 
                                        (current_config.channels * 
                                         current_config.bits_per_sample / 8));
        
        if (frames_written < 0) {
            // Underrun or error
            if (frames_written == -EPIPE) {
                snd_pcm_prepare(pcm_handle);
            } else {
                break;
            }
        } else if (frames_written > 0) {
            // Move buffer pointer or handle as needed
            // For this example, we just loop
        }
        
        // Check if we should stop
        snd_pcm_delay(pcm_handle, &delay);
        if (delay < 0) {
            break;
        }
    }
    
    return NULL;
}

// Play audio
int audio_play(AudioBuffer* buffer) {
    if (!is_initialized || !pcm_handle) {
        set_error("Audio not initialized");
        return -1;
    }
    
    if (!buffer || !buffer->buffer || buffer->data_size == 0) {
        set_error("Invalid buffer");
        return -1;
    }
    
    // Stop any ongoing playback
    audio_stop();
    
    // Start playback
    playback_running = true;
    current_config.is_playing = true;
    
    if (pthread_create(&playback_thread, NULL, 
                       playback_thread_func, buffer) != 0) {
        set_error("Cannot create playback thread");
        playback_running = false;
        current_config.is_playing = false;
        return -1;
    }
    
    return 0;
}

// Stop audio
int audio_stop(void) {
    if (!is_initialized) {
        return 0;
    }
    
    playback_running = false;
    if (current_config.is_playing) {
        pthread_join(playback_thread, NULL);
        current_config.is_playing = false;
    }
    
    if (pcm_handle) {
        snd_pcm_drop(pcm_handle);
        snd_pcm_prepare(pcm_handle);
    }
    
    return 0;
}

// Set volume (0.0 to 1.0)
int audio_set_volume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    current_config.volume = volume;
    
    // Use ALSA softvol or mixer if available
    // For now, just store value
    return 0;
}

float audio_get_volume(void) {
    return current_config.volume;
}

// Create audio buffer with sine wave
int audio_fill_sine_wave(AudioBuffer* buffer, float frequency_hz, float amplitude) {
    if (!buffer || !buffer->buffer) {
        set_error("Invalid buffer");
        return -1;
    }
    
    if (amplitude < 0.0f || amplitude > 1.0f) {
        amplitude = 0.5f;
    }
    
    int16_t* samples = (int16_t*)buffer->buffer;
    uint32_t num_samples = buffer->data_size / sizeof(int16_t);
    float max_amplitude = amplitude * 32767.0f;
    float phase = 0.0f;
    float phase_increment = 2.0f * M_PI * frequency_hz / buffer->sample_rate;
    
    for (uint32_t i = 0; i < num_samples; i++) {
        samples[i] = (int16_t)(sinf(phase) * max_amplitude);
        phase += phase_increment;
        if (phase >= 2.0f * M_PI) {
            phase -= 2.0f * M_PI;
        }
    }
    
    return 0;
}

// Cleanup
void audio_cleanup(void) {
    audio_stop();
    if (pcm_handle) {
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
    }
    is_initialized = false;
}
