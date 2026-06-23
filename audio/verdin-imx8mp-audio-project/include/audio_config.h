#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

// ============================================
// Audio Configuration Macros
// ============================================

// Default Audio Settings
#define AUDIO_DEFAULT_SAMPLE_RATE 48000
#define AUDIO_DEFAULT_CHANNELS 2
#define AUDIO_DEFAULT_BITS 16
#define AUDIO_DEFAULT_BUFFER_SIZE 4096
#define AUDIO_DEFAULT_DEVICE "default"

// Audio Format Constants
#define AUDIO_FORMAT_PCM 1
#define AUDIO_FORMAT_IEEE_FLOAT 3

// Error Codes
#define AUDIO_SUCCESS 0
#define AUDIO_ERR_OPEN -1
#define AUDIO_ERR_PARAMS -2
#define AUDIO_ERR_PREPARE -3
#define AUDIO_ERR_WRITE -4
#define AUDIO_ERR_READ -5
#define AUDIO_ERR_BUFFER -6
#define AUDIO_ERR_THREAD -7
#define AUDIO_ERR_INIT -8
#define AUDIO_ERR_DEVICE -9
#define AUDIO_ERR_FORMAT -10

// Performance Settings
#define AUDIO_RECORD_CHUNK_SIZE 4096
#define AUDIO_PLAYBACK_CHUNK_SIZE 4096
#define AUDIO_MAX_DEVICE_NAME 64
#define AUDIO_MAX_BUFFER_SIZE (1024 * 1024 * 10) // 10MB max

// Debug Settings
#ifdef DEBUG
    #define AUDIO_DEBUG_PRINT(fmt, ...) \
        printf("[AUDIO_DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define AUDIO_DEBUG_PRINT(fmt, ...)
#endif

// Version Information
#define AUDIO_VERSION_MAJOR 1
#define AUDIO_VERSION_MINOR 4
#define AUDIO_VERSION_PATCH 0
#define AUDIO_VERSION_STRING "1.4.0"

#endif // AUDIO_CONFIG_H
