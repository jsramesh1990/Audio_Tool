#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include "utils.h"
#include "audio_config.h"

// ============================================
// LOGGING UTILITIES
// ============================================

static LogLevel current_log_level = LOG_LEVEL_INFO;
static FILE* log_file = NULL;

void utils_set_log_level(LogLevel level) {
    current_log_level = level;
}

void utils_set_log_file(const char* filename) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    
    if (filename) {
        log_file = fopen(filename, "a");
        if (!log_file) {
            fprintf(stderr, "Warning: Could not open log file: %s\n", filename);
        }
    }
}

void utils_log(LogLevel level, const char* function, const char* format, ...) {
    if (level < current_log_level) {
        return;
    }
    
    const char* level_str;
    switch (level) {
        case LOG_LEVEL_ERROR:   level_str = "ERROR"; break;
        case LOG_LEVEL_WARNING: level_str = "WARN"; break;
        case LOG_LEVEL_INFO:    level_str = "INFO"; break;
        case LOG_LEVEL_DEBUG:   level_str = "DEBUG"; break;
        default:                level_str = "UNKNOWN"; break;
    }
    
    // Get current time
    time_t now;
    struct tm* tm_info;
    char time_str[32];
    time(&now);
    tm_info = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Format message
    va_list args;
    va_start(args, format);
    char message[512];
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // Log to stdout
    printf("[%s] [%s] [%s] %s\n", time_str, level_str, function, message);
    fflush(stdout);
    
    // Log to file if available
    if (log_file) {
        fprintf(log_file, "[%s] [%s] [%s] %s\n", time_str, level_str, function, message);
        fflush(log_file);
    }
}

// ============================================
// TIME UTILITIES
// ============================================

uint64_t utils_get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL);
}

uint64_t utils_get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec * 1000000ULL + tv.tv_usec);
}

void utils_sleep_ms(uint32_t ms) {
    usleep(ms * 1000);
}

void utils_sleep_us(uint32_t us) {
    usleep(us);
}

// ============================================
// MEMORY UTILITIES
// ============================================

void* utils_malloc_safe(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (!ptr) {
        utils_log(LOG_LEVEL_ERROR, __FUNCTION__, 
                  "Failed to allocate %zu bytes", size);
        return NULL;
    }
    
    memset(ptr, 0, size);
    return ptr;
}

void* utils_realloc_safe(void* ptr, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    void* new_ptr = realloc(ptr, new_size);
    if (!new_ptr) {
        utils_log(LOG_LEVEL_ERROR, __FUNCTION__, 
                  "Failed to reallocate %zu bytes", new_size);
        return NULL;
    }
    
    return new_ptr;
}

void utils_free_safe(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

// ============================================
// BUFFER UTILITIES
// ============================================

int utils_buffer_convert_format(const void* src, void* dst, 
                                uint32_t num_samples,
                                AudioFormat src_format, 
                                AudioFormat dst_format) {
    if (!src || !dst || num_samples == 0) {
        return -1;
    }
    
    // Convert based on formats
    // This is a simplified example - expand as needed
    
    if (src_format == dst_format) {
        // Same format, just copy
        size_t sample_size = 0;
        switch (src_format) {
            case AUDIO_FORMAT_S16: sample_size = 2; break;
            case AUDIO_FORMAT_S24: sample_size = 3; break;
            case AUDIO_FORMAT_S32: 
            case AUDIO_FORMAT_FLOAT: sample_size = 4; break;
            default: return -1;
        }
        memcpy(dst, src, num_samples * sample_size);
        return 0;
    }
    
    // Convert from S16 to FLOAT
    if (src_format == AUDIO_FORMAT_S16 && dst_format == AUDIO_FORMAT_FLOAT) {
        const int16_t* src_samples = (const int16_t*)src;
        float* dst_samples = (float*)dst;
        for (uint32_t i = 0; i < num_samples; i++) {
            dst_samples[i] = src_samples[i] / 32768.0f;
        }
        return 0;
    }
    
    // Convert from FLOAT to S16
    if (src_format == AUDIO_FORMAT_FLOAT && dst_format == AUDIO_FORMAT_S16) {
        const float* src_samples = (const float*)src;
        int16_t* dst_samples = (int16_t*)dst;
        for (uint32_t i = 0; i < num_samples; i++) {
            float val = src_samples[i] * 32767.0f;
            if (val > 32767.0f) val = 32767.0f;
            if (val < -32768.0f) val = -32768.0f;
            dst_samples[i] = (int16_t)val;
        }
        return 0;
    }
    
    // Add more conversions as needed (S24, S32, etc.)
    
    utils_log(LOG_LEVEL_WARNING, __FUNCTION__,
              "Unsupported format conversion: %d -> %d", src_format, dst_format);
    return -1;
}

int utils_buffer_mix(void* buffer1, const void* buffer2, 
                     uint32_t num_samples, AudioFormat format) {
    if (!buffer1 || !buffer2 || num_samples == 0) {
        return -1;
    }
    
    switch (format) {
        case AUDIO_FORMAT_S16: {
            int16_t* b1 = (int16_t*)buffer1;
            const int16_t* b2 = (const int16_t*)buffer2;
            for (uint32_t i = 0; i < num_samples; i++) {
                int32_t mixed = (int32_t)b1[i] + (int32_t)b2[i];
                // Clip to avoid overflow
                if (mixed > 32767) mixed = 32767;
                if (mixed < -32768) mixed = -32768;
                b1[i] = (int16_t)mixed;
            }
            break;
        }
        case AUDIO_FORMAT_FLOAT: {
            float* b1 = (float*)buffer1;
            const float* b2 = (const float*)buffer2;
            for (uint32_t i = 0; i < num_samples; i++) {
                b1[i] = b1[i] + b2[i];
                if (b1[i] > 1.0f) b1[i] = 1.0f;
                if (b1[i] < -1.0f) b1[i] = -1.0f;
            }
            break;
        }
        default:
            return -1;
    }
    
    return 0;
}

// ============================================
// FILE UTILITIES
// ============================================

bool utils_file_exists(const char* filename) {
    if (!filename) return false;
    return access(filename, F_OK) == 0;
}

size_t utils_file_get_size(const char* filename) {
    if (!filename) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    
    return size;
}

int utils_file_read_all(const char* filename, void** out_data, size_t* out_size) {
    if (!filename || !out_data || !out_size) {
        return -1;
    }
    
    size_t file_size = utils_file_get_size(filename);
    if (file_size == 0) {
        return -1;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return -1;
    }
    
    void* data = malloc(file_size);
    if (!data) {
        fclose(file);
        return -1;
    }
    
    size_t bytes_read = fread(data, 1, file_size, file);
    fclose(file);
    
    if (bytes_read != file_size) {
        free(data);
        return -1;
    }
    
    *out_data = data;
    *out_size = file_size;
    return 0;
}

int utils_file_write_all(const char* filename, const void* data, size_t size) {
    if (!filename || !data || size == 0) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return -1;
    }
    
    size_t bytes_written = fwrite(data, 1, size, file);
    fclose(file);
    
    return (bytes_written == size) ? 0 : -1;
}

// ============================================
// STRING UTILITIES
// ============================================

void utils_string_trim(char* str) {
    if (!str) return;
    
    // Trim leading whitespace
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    
    // Trim trailing whitespace
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    
    // Move trimmed string to beginning if needed
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

char* utils_string_duplicate(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* dup = (char*)malloc(len);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

char* utils_string_join(const char* sep, int count, ...) {
    if (count <= 0 || !sep) return NULL;
    
    va_list args;
    va_start(args, count);
    
    // Calculate total length
    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        const char* str = va_arg(args, const char*);
        if (str) {
            total_len += strlen(str);
            if (i < count - 1) {
                total_len += strlen(sep);
            }
        }
    }
    va_end(args);
    
    if (total_len == 0) return NULL;
    
    char* result = (char*)malloc(total_len + 1);
    if (!result) return NULL;
    
    result[0] = '\0';
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        const char* str = va_arg(args, const char*);
        if (str) {
            strcat(result, str);
            if (i < count - 1) {
                strcat(result, sep);
            }
        }
    }
    va_end(args);
    
    return result;
}

// ============================================
// MATH UTILITIES
// ============================================

float utils_lerp(float a, float b, float t) {
    if (t <= 0.0f) return a;
    if (t >= 1.0f) return b;
    return a + t * (b - a);
}

float utils_clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float utils_db_to_linear(float db) {
    return powf(10.0f, db / 20.0f);
}

float utils_linear_to_db(float linear) {
    if (linear <= 0.0f) return -INFINITY;
    return 20.0f * log10f(linear);
}

// ============================================
// DSP UTILITIES
// ============================================

void utils_apply_volume(void* buffer, uint32_t num_samples, 
                        AudioFormat format, float volume) {
    if (!buffer || num_samples == 0 || volume == 1.0f) return;
    
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 2.0f) volume = 2.0f; // Allow some boost
    
    switch (format) {
        case AUDIO_FORMAT_S16: {
            int16_t* samples = (int16_t*)buffer;
            for (uint32_t i = 0; i < num_samples; i++) {
                float val = (float)samples[i] * volume;
                if (val > 32767.0f) val = 32767.0f;
                if (val < -32768.0f) val = -32768.0f;
                samples[i] = (int16_t)val;
            }
            break;
        }
        case AUDIO_FORMAT_FLOAT: {
            float* samples = (float*)buffer;
            for (uint32_t i = 0; i < num_samples; i++) {
                samples[i] *= volume;
                if (samples[i] > 1.0f) samples[i] = 1.0f;
                if (samples[i] < -1.0f) samples[i] = -1.0f;
            }
            break;
        }
        default:
            utils_log(LOG_LEVEL_WARNING, __FUNCTION__,
                     "Unsupported format for volume application");
    }
}

void utils_generate_silence(void* buffer, uint32_t num_samples, 
                           AudioFormat format) {
    if (!buffer || num_samples == 0) return;
    
    size_t sample_size = 0;
    switch (format) {
        case AUDIO_FORMAT_S16: sample_size = 2; break;
        case AUDIO_FORMAT_S24: sample_size = 3; break;
        case AUDIO_FORMAT_S32: sample_size = 4; break;
        case AUDIO_FORMAT_FLOAT: sample_size = 4; break;
        default: return;
    }
    
    memset(buffer, 0, num_samples * sample_size);
}

// ============================================
// SYSTEM UTILITIES
// ============================================

void utils_print_system_info(void) {
    printf("\n=== System Information ===\n");
    
    // CPU info
    FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo) {
        char line[256];
        while (fgets(line, sizeof(line), cpuinfo)) {
            if (strstr(line, "Model") || strstr(line, "CPU")) {
                printf("%s", line);
            }
        }
        fclose(cpuinfo);
    }
    
    // Memory info
    FILE* meminfo = fopen("/proc/meminfo", "r");
    if (meminfo) {
        char line[256];
        while (fgets(line, sizeof(line), meminfo)) {
            if (strstr(line, "MemTotal") || strstr(line, "MemFree")) {
                printf("%s", line);
            }
        }
        fclose(meminfo);
    }
    
    printf("==========================\n\n");
}

int utils_get_cpu_usage(void) {
    static uint64_t prev_idle = 0;
    static uint64_t prev_total = 0;
    static int first_call = 1;
    
    FILE* stat = fopen("/proc/stat", "r");
    if (!stat) return -1;
    
    char line[256];
    uint64_t user, nice, system, idle, iowait, irq, softirq;
    
    if (fgets(line, sizeof(line), stat)) {
        sscanf(line, "cpu  %lu %lu %lu %lu %lu %lu %lu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    }
    fclose(stat);
    
    uint64_t total = user + nice + system + idle + iowait + irq + softirq;
    
    if (first_call) {
        prev_idle = idle;
        prev_total = total;
        first_call = 0;
        return 0;
    }
    
    uint64_t diff_total = total - prev_total;
    uint64_t diff_idle = idle - prev_idle;
    
    prev_idle = idle;
    prev_total = total;
    
    if (diff_total == 0) return 0;
    
    return (int)((diff_total - diff_idle) * 100 / diff_total);
}

// ============================================
// TEST FUNCTIONS
// ============================================

#ifdef ENABLE_TESTS

void utils_run_tests(void) {
    printf("\n=== Running Utility Tests ===\n");
    
    // Test logging
    utils_set_log_level(LOG_LEVEL_DEBUG);
    utils_log(LOG_LEVEL_INFO, "test", "Utility test started");
    utils_log(LOG_LEVEL_DEBUG, "test", "This is a debug message");
    utils_log(LOG_LEVEL_ERROR, "test", "This is an error message");
    
    // Test time functions
    uint64_t ts_ms = utils_get_timestamp_ms();
    uint64_t ts_us = utils_get_timestamp_us();
    printf("Timestamp: %llu ms, %llu us\n", 
           (unsigned long long)ts_ms, (unsigned long long)ts_us);
    
    // Test memory functions
    void* ptr = utils_malloc_safe(100);
    if (ptr) {
        printf("Memory allocated successfully\n");
        utils_free_safe(&ptr);
        printf("Memory freed successfully\n");
    }
    
    // Test string functions
    char test_str[] = "  Hello World!  ";
    utils_string_trim(test_str);
    printf("Trimmed string: '%s'\n", test_str);
    
    char* joined = utils_string_join(", ", 3, "apple", "banana", "orange");
    if (joined) {
        printf("Joined string: %s\n", joined);
        free(joined);
    }
    
    // Test math functions
    float lerp_test = utils_lerp(0.0f, 10.0f, 0.5f);
    printf("Lerp: %f\n", lerp_test);
    
    float db_test = utils_linear_to_db(0.5f);
    printf("0.5 linear = %.2f dB\n", db_test);
    
    // Test DSP functions
    int16_t test_samples[] = {1000, -2000, 3000, -4000};
    utils_apply_volume(test_samples, 4, AUDIO_FORMAT_S16, 0.5f);
    printf("Volume reduced samples: %d, %d, %d, %d\n",
           test_samples[0], test_samples[1], test_samples[2], test_samples[3]);
    
    // Test CPU usage
    int cpu_usage = utils_get_cpu_usage();
    printf("CPU usage: %d%%\n", cpu_usage);
    
    printf("=== Tests Completed ===\n\n");
}

#endif // ENABLE_TESTS
