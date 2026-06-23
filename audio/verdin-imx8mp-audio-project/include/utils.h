#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

// ============================================
// LOGGING
// ============================================

typedef enum {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_DEBUG = 3
} LogLevel;

void utils_set_log_level(LogLevel level);
void utils_set_log_file(const char* filename);
void utils_log(LogLevel level, const char* function, const char* format, ...);

// Convenience macros
#define LOG_ERROR(fmt, ...) utils_log(LOG_LEVEL_ERROR, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) utils_log(LOG_LEVEL_WARNING, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) utils_log(LOG_LEVEL_INFO, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) utils_log(LOG_LEVEL_DEBUG, __FUNCTION__, fmt, ##__VA_ARGS__)

// ============================================
// TIME UTILITIES
// ============================================

uint64_t utils_get_timestamp_ms(void);
uint64_t utils_get_timestamp_us(void);
void utils_sleep_ms(uint32_t ms);
void utils_sleep_us(uint32_t us);

// ============================================
// MEMORY UTILITIES
// ============================================

void* utils_malloc_safe(size_t size);
void* utils_realloc_safe(void* ptr, size_t new_size);
void utils_free_safe(void** ptr);

// ============================================
// BUFFER UTILITIES
// ============================================

typedef enum {
    AUDIO_FORMAT_S16 = 0,
    AUDIO_FORMAT_S24 = 1,
    AUDIO_FORMAT_S32 = 2,
    AUDIO_FORMAT_FLOAT = 3
} AudioFormat;

int utils_buffer_convert_format(const void* src, void* dst, 
                                uint32_t num_samples,
                                AudioFormat src_format, 
                                AudioFormat dst_format);

int utils_buffer_mix(void* buffer1, const void* buffer2, 
                     uint32_t num_samples, AudioFormat format);

// ============================================
// FILE UTILITIES
// ============================================

bool utils_file_exists(const char* filename);
size_t utils_file_get_size(const char* filename);
int utils_file_read_all(const char* filename, void** out_data, size_t* out_size);
int utils_file_write_all(const char* filename, const void* data, size_t size);

// ============================================
// STRING UTILITIES
// ============================================

void utils_string_trim(char* str);
char* utils_string_duplicate(const char* str);
char* utils_string_join(const char* sep, int count, ...);

// ============================================
// MATH UTILITIES
// ============================================

float utils_lerp(float a, float b, float t);
float utils_clamp(float value, float min, float max);
float utils_db_to_linear(float db);
float utils_linear_to_db(float linear);

// ============================================
// DSP UTILITIES
// ============================================

void utils_apply_volume(void* buffer, uint32_t num_samples, 
                        AudioFormat format, float volume);
void utils_generate_silence(void* buffer, uint32_t num_samples, 
                           AudioFormat format);

// ============================================
// SYSTEM UTILITIES
// ============================================

void utils_print_system_info(void);
int utils_get_cpu_usage(void);

// ============================================
// TEST
// ============================================

#ifdef ENABLE_TESTS
void utils_run_tests(void);
#endif

#endif // UTILS_H
