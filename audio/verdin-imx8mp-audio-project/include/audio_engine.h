#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "audio_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================
// Type Definitions
// ============================================

typedef struct {
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint32_t buffer_size;
    char device_name[64];
    float volume;
    bool is_playing;
    bool is_recording;
    bool use_dma;
    uint32_t period_size;
    uint32_t periods_count;
} AudioConfig;

typedef struct {
    void* buffer;
    size_t buffer_size;
    size_t data_size;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint64_t timestamp;
    bool is_allocated;
} AudioBuffer;

// ============================================
// Core Functions
// ============================================

/**
 * Initialize the audio system
 * @param config Audio configuration parameters
 * @return 0 on success, negative error code on failure
 */
int audio_init(AudioConfig* config);

/**
 * Play audio from buffer
 * @param buffer Audio buffer to play
 * @return 0 on success, negative error code on failure
 */
int audio_play(AudioBuffer* buffer);

/**
 * Record audio to buffer
 * @param buffer Buffer to store recorded audio
 * @param duration_ms Recording duration in milliseconds
 * @return 0 on success, negative error code on failure
 */
int audio_record(AudioBuffer* buffer, uint32_t duration_ms);

/**
 * Stop audio playback or recording
 * @return 0 on success, negative error code on failure
 */
int audio_stop(void);

/**
 * Pause audio playback
 * @return 0 on success, negative error code on failure
 */
int audio_pause(void);

/**
 * Resume audio playback
 * @return 0 on success, negative error code on failure
 */
int audio_resume(void);

/**
 * Get current playback position
 * @return Current position in milliseconds
 */
uint32_t audio_get_position_ms(void);

/**
 * Get total playback duration
 * @return Total duration in milliseconds
 */
uint32_t audio_get_duration_ms(void);

/**
 * Set audio volume
 * @param volume Volume level (0.0 to 1.0)
 * @return 0 on success, negative error code on failure
 */
int audio_set_volume(float volume);

/**
 * Get current volume
 * @return Current volume level (0.0 to 1.0)
 */
float audio_get_volume(void);

/**
 * Check if audio is playing
 * @return true if playing, false otherwise
 */
bool audio_is_playing(void);

/**
 * Check if audio is recording
 * @return true if recording, false otherwise
 */
bool audio_is_recording(void);

/**
 * Clean up and close audio device
 */
void audio_cleanup(void);

// ============================================
// Buffer Management
// ============================================

/**
 * Create audio buffer
 * @param sample_rate Sample rate in Hz
 * @param channels Number of audio channels
 * @param bits_per_sample Bit depth (16, 24, 32)
 * @param duration_ms Buffer duration in milliseconds
 * @return Pointer to AudioBuffer on success, NULL on failure
 */
AudioBuffer* audio_create_buffer(uint32_t sample_rate, uint8_t channels, 
                                  uint8_t bits_per_sample, uint32_t duration_ms);

/**
 * Free audio buffer
 * @param buffer Buffer to free
 */
void audio_free_buffer(AudioBuffer* buffer);

/**
 * Clear audio buffer (fill with silence)
 * @param buffer Buffer to clear
 * @return 0 on success, negative error code on failure
 */
int audio_clear_buffer(AudioBuffer* buffer);

/**
 * Copy audio buffer
 * @param dest Destination buffer
 * @param src Source buffer
 * @return 0 on success, negative error code on failure
 */
int audio_copy_buffer(AudioBuffer* dest, const AudioBuffer* src);

/**
 * Fill buffer with sine wave
 * @param buffer Buffer to fill
 * @param frequency_hz Frequency in Hz
 * @param amplitude Amplitude (0.0 to 1.0)
 * @return 0 on success, negative error code on failure
 */
int audio_fill_sine_wave(AudioBuffer* buffer, float frequency_hz, float amplitude);

/**
 * Fill buffer with square wave
 * @param buffer Buffer to fill
 * @param frequency_hz Frequency in Hz
 * @param amplitude Amplitude (0.0 to 1.0)
 * @return 0 on success, negative error code on failure
 */
int audio_fill_square_wave(AudioBuffer* buffer, float frequency_hz, float amplitude);

/**
 * Fill buffer with sawtooth wave
 * @param buffer Buffer to fill
 * @param frequency_hz Frequency in Hz
 * @param amplitude Amplitude (0.0 to 1.0)
 * @return 0 on success, negative error code on failure
 */
int audio_fill_sawtooth_wave(AudioBuffer* buffer, float frequency_hz, float amplitude);

/**
 * Fill buffer with white noise
 * @param buffer Buffer to fill
 * @param amplitude Amplitude (0.0 to 1.0)
 * @return 0 on success, negative error code on failure
 */
int audio_fill_noise(AudioBuffer* buffer, float amplitude);

// ============================================
// Error Handling
// ============================================

/**
 * Get last error message
 * @return String describing the last error
 */
const char* audio_get_error(void);

/**
 * Clear error message
 */
void audio_clear_error(void);

/**
 * Get error code for last operation
 * @return Error code
 */
int audio_get_error_code(void);

// ============================================
// Utility Functions
// ============================================

/**
 * Get audio engine version
 * @return Version string
 */
const char* audio_get_version(void);

/**
 * Get audio device info
 * @param info_buffer Buffer to store info string
 * @param buffer_size Size of info buffer
 * @return 0 on success, negative error code on failure
 */
int audio_get_device_info(char* info_buffer, size_t buffer_size);

/**
 * List available audio devices
 * @param device_list Buffer to store device list
 * @param max_devices Maximum number of devices
 * @return Number of devices found, negative on error
 */
int audio_list_devices(char device_list[][64], int max_devices);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_ENGINE_H
