#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint32_t buffer_size;
    char device_name[64];
    float volume;
    bool is_playing;
    bool is_recording;
} AudioConfig;

typedef struct {
    void* buffer;
    size_t buffer_size;
    size_t data_size;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
} AudioBuffer;

// Core Functions
int audio_init(AudioConfig* config);
int audio_play(AudioBuffer* buffer);
int audio_record(AudioBuffer* buffer, uint32_t duration_ms);
int audio_stop(void);
int audio_set_volume(float volume);
float audio_get_volume(void);

// Buffer Management
AudioBuffer* audio_create_buffer(uint32_t sample_rate, uint8_t channels, uint8_t bits, uint32_t duration_ms);
void audio_free_buffer(AudioBuffer* buffer);
int audio_fill_sine_wave(AudioBuffer* buffer, float frequency_hz, float amplitude);

// Error Handling
const char* audio_get_error(void);
void audio_clear_error(void);

#endif // AUDIO_ENGINE_H
