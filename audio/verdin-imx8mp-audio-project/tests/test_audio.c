#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include "../include/audio_engine.h"
#include "../include/wav_loader.h"
#include "../include/utils.h"

// Global test state
static int tests_passed = 0;
static int tests_failed = 0;
static volatile int test_running = 1;

// Test macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "❌ FAILED: %s (line %d): %s\n", __FUNCTION__, __LINE__, message); \
            tests_failed++; \
            return -1; \
        } \
    } while(0)

#define TEST_PASS(message) \
    do { \
        printf("✅ PASS: %s\n", message); \
        tests_passed++; \
    } while(0)

#define TEST_START(name) \
    do { \
        printf("\n=== Testing %s ===\n", name); \
    } while(0)

// Signal handler for tests
void test_signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n⚠️ Test interrupted by user\n");
        test_running = 0;
        audio_stop();
        audio_cleanup();
        exit(0);
    }
}

// ============================================
// TEST: Audio Initialization
// ============================================
int test_audio_init(void) {
    TEST_START("Audio Initialization");
    
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.5f,
        .is_playing = false,
        .is_recording = false
    };
    strcpy(config.device_name, "default");
    
    // Test 1: Initialize with valid config
    int result = audio_init(&config);
    TEST_ASSERT(result == 0, "Audio initialization failed");
    TEST_PASS("Audio initialized successfully");
    
    // Test 2: Get configuration
    TEST_ASSERT(config.sample_rate == 48000, "Sample rate mismatch");
    TEST_ASSERT(config.channels == 2, "Channel count mismatch");
    TEST_ASSERT(config.bits_per_sample == 16, "Bits per sample mismatch");
    TEST_PASS("Configuration verified");
    
    // Test 3: Set and get volume
    result = audio_set_volume(0.75f);
    TEST_ASSERT(result == 0, "Volume set failed");
    float vol = audio_get_volume();
    TEST_ASSERT(fabs(vol - 0.75f) < 0.01f, "Volume get mismatch");
    TEST_PASS("Volume control working");
    
    audio_cleanup();
    TEST_PASS("Cleanup successful");
    return 0;
}

// ============================================
// TEST: Sine Wave Generation
// ============================================
int test_sine_wave_generation(void) {
    TEST_START("Sine Wave Generation");
    
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 1,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.5f
    };
    strcpy(config.device_name, "default");
    
    // Initialize audio
    TEST_ASSERT(audio_init(&config) == 0, "Audio init failed");
    
    // Create buffer for 1 second of audio
    AudioBuffer* buffer = audio_create_buffer(48000, 1, 16, 1000);
    TEST_ASSERT(buffer != NULL, "Buffer creation failed");
    TEST_PASS("Buffer created successfully");
    
    // Fill with sine wave at 440Hz
    int result = audio_fill_sine_wave(buffer, 440.0f, 0.5f);
    TEST_ASSERT(result == 0, "Sine wave generation failed");
    TEST_PASS("Sine wave generated");
    
    // Verify data
    int16_t* samples = (int16_t*)buffer->buffer;
    uint32_t num_samples = buffer->data_size / sizeof(int16_t);
    
    // Check first few samples
    bool found_positive = false;
    bool found_negative = false;
    for (uint32_t i = 0; i < 100 && i < num_samples; i++) {
        if (samples[i] > 1000) found_positive = true;
        if (samples[i] < -1000) found_negative = true;
    }
    TEST_ASSERT(found_positive && found_negative, "Sine wave not oscillating correctly");
    TEST_PASS("Sine wave verified");
    
    audio_free_buffer(buffer);
    audio_cleanup();
    return 0;
}

// ============================================
// TEST: WAV File Operations
// ============================================
int test_wav_operations(void) {
    TEST_START("WAV File Operations");
    
    AudioConfig config = {
        .sample_rate = 44100,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.5f
    };
    strcpy(config.device_name, "default");
    
    audio_init(&config);
    
    // Test 1: Create and save WAV
    AudioBuffer* buffer = audio_create_buffer(44100, 2, 16, 2000);
    TEST_ASSERT(buffer != NULL, "Buffer creation failed");
    
    audio_fill_sine_wave(buffer, 440.0f, 0.5f);
    
    const char* test_file = "/tmp/test_audio.wav";
    int result = wav_save_to_file(test_file, buffer);
    TEST_ASSERT(result == 0, "WAV save failed");
    TEST_PASS("WAV file saved successfully");
    
    // Test 2: Load WAV file
    AudioBuffer* loaded_buffer = NULL;
    result = wav_load_from_file(test_file, &loaded_buffer);
    TEST_ASSERT(result == 0, "WAV load failed");
    TEST_PASS("WAV file loaded successfully");
    
    // Test 3: Verify data
    TEST_ASSERT(loaded_buffer != NULL, "Loaded buffer is NULL");
    TEST_ASSERT(loaded_buffer->sample_rate == 44100, "Sample rate mismatch");
    TEST_ASSERT(loaded_buffer->channels == 2, "Channel count mismatch");
    TEST_ASSERT(loaded_buffer->bits_per_sample == 16, "Bits per sample mismatch");
    TEST_PASS("WAV data verified");
    
    // Cleanup
    audio_free_buffer(buffer);
    audio_free_buffer(loaded_buffer);
    audio_cleanup();
    
    // Remove test file
    remove(test_file);
    return 0;
}

// ============================================
// TEST: Buffer Operations
// ============================================
int test_buffer_operations(void) {
    TEST_START("Buffer Operations");
    
    // Test 1: Create buffer
    AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 100);
    TEST_ASSERT(buffer != NULL, "Buffer creation failed");
    TEST_ASSERT(buffer->buffer != NULL, "Buffer data allocation failed");
    TEST_PASS("Buffer created");
    
    // Test 2: Buffer properties
    TEST_ASSERT(buffer->sample_rate == 48000, "Sample rate mismatch");
    TEST_ASSERT(buffer->channels == 2, "Channel mismatch");
    TEST_ASSERT(buffer->bits_per_sample == 16, "Bits mismatch");
    uint32_t expected_size = (48000 * 100 / 1000) * 2 * 2; // samples * channels * bytes
    TEST_ASSERT(buffer->data_size == expected_size, "Buffer size mismatch");
    TEST_PASS("Buffer properties correct");
    
    // Test 3: Fill with silence
    memset(buffer->buffer, 0, buffer->data_size);
    int16_t* samples = (int16_t*)buffer->buffer;
    bool all_zero = true;
    for (uint32_t i = 0; i < buffer->data_size / sizeof(int16_t); i++) {
        if (samples[i] != 0) {
            all_zero = false;
            break;
        }
    }
    TEST_ASSERT(all_zero, "Silence buffer not all zero");
    TEST_PASS("Silence buffer verified");
    
    // Test 4: Fill with sine wave
    audio_fill_sine_wave(buffer, 1000.0f, 0.3f);
    bool has_signal = false;
    for (uint32_t i = 0; i < 100; i++) {
        if (samples[i] != 0) {
            has_signal = true;
            break;
        }
    }
    TEST_ASSERT(has_signal, "Sine wave buffer is all zero");
    TEST_PASS("Sine wave buffer verified");
    
    audio_free_buffer(buffer);
    return 0;
}

// ============================================
// TEST: Volume Control
// ============================================
int test_volume_control(void) {
    TEST_START("Volume Control");
    
    // Create test buffer
    AudioBuffer* buffer = audio_create_buffer(48000, 1, 16, 100);
    TEST_ASSERT(buffer != NULL, "Buffer creation failed");
    
    // Fill with sine wave
    audio_fill_sine_wave(buffer, 440.0f, 1.0f);
    
    int16_t* samples = (int16_t*)buffer->buffer;
    int16_t original_sample = samples[0];
    
    // Apply volume reduction
    utils_apply_volume(buffer->buffer, buffer->data_size / sizeof(int16_t), 
                      AUDIO_FORMAT_S16, 0.5f);
    
    int16_t reduced_sample = samples[0];
    float ratio = (float)reduced_sample / original_sample;
    
    TEST_ASSERT(fabs(ratio - 0.5f) < 0.1f, "Volume reduction incorrect");
    TEST_PASS("Volume control working");
    
    audio_free_buffer(buffer);
    return 0;
}

// ============================================
// TEST: Format Conversion
// ============================================
int test_format_conversion(void) {
    TEST_START("Format Conversion");
    
    // S16 to Float conversion
    const uint32_t num_samples = 10;
    int16_t s16_data[] = {32767, 16384, 0, -16384, -32768, 10000, -10000, 0, 0, 0};
    float float_data[10] = {0};
    
    int result = utils_buffer_convert_format(s16_data, float_data, num_samples,
                                            AUDIO_FORMAT_S16, AUDIO_FORMAT_FLOAT);
    TEST_ASSERT(result == 0, "S16->Float conversion failed");
    
    // Verify conversion
    TEST_ASSERT(fabs(float_data[0] - 1.0f) < 0.01f, "Float conversion incorrect");
    TEST_ASSERT(fabs(float_data[2] - 0.0f) < 0.01f, "Float conversion incorrect");
    TEST_ASSERT(fabs(float_data[4] - (-1.0f)) < 0.01f, "Float conversion incorrect");
    TEST_PASS("S16 to Float conversion verified");
    
    // Float to S16 conversion
    int16_t s16_converted[10] = {0};
    result = utils_buffer_convert_format(float_data, s16_converted, num_samples,
                                        AUDIO_FORMAT_FLOAT, AUDIO_FORMAT_S16);
    TEST_ASSERT(result == 0, "Float->S16 conversion failed");
    TEST_ASSERT(s16_converted[0] == 32767, "Float->S16 conversion incorrect");
    TEST_ASSERT(s16_converted[2] == 0, "Float->S16 conversion incorrect");
    TEST_PASS("Float to S16 conversion verified");
    
    return 0;
}

// ============================================
// TEST: Performance
// ============================================
int test_performance(void) {
    TEST_START("Performance Test");
    
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.5f
    };
    strcpy(config.device_name, "default");
    
    audio_init(&config);
    
    // Test buffer creation performance
    const int NUM_BUFFERS = 10;
    AudioBuffer* buffers[NUM_BUFFERS];
    uint64_t start_time = utils_get_timestamp_us();
    
    for (int i = 0; i < NUM_BUFFERS; i++) {
        buffers[i] = audio_create_buffer(48000, 2, 16, 1000);
        TEST_ASSERT(buffers[i] != NULL, "Buffer creation failed");
    }
    
    uint64_t end_time = utils_get_timestamp_us();
    uint64_t elapsed = end_time - start_time;
    printf("⏱️  Created %d buffers in %.2f ms\n", NUM_BUFFERS, elapsed / 1000.0);
    TEST_PASS("Buffer creation performance acceptable");
    
    // Cleanup
    for (int i = 0; i < NUM_BUFFERS; i++) {
        audio_free_buffer(buffers[i]);
    }
    
    audio_cleanup();
    return 0;
}

// ============================================
// TEST: Error Handling
// ============================================
int test_error_handling(void) {
    TEST_START("Error Handling");
    
    // Test 1: NULL config
    int result = audio_init(NULL);
    TEST_ASSERT(result != 0, "Audio init should fail with NULL config");
    TEST_PASS("NULL config handled correctly");
    
    // Test 2: Invalid device
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16
    };
    strcpy(config.device_name, "invalid_device_xyz");
    result = audio_init(&config);
    TEST_ASSERT(result != 0, "Invalid device should fail");
    TEST_PASS("Invalid device handled correctly");
    
    // Test 3: Invalid buffer for playback
    audio_init(&config);
    result = audio_play(NULL);
    TEST_ASSERT(result != 0, "Play with NULL buffer should fail");
    TEST_PASS("NULL buffer handled correctly");
    
    audio_cleanup();
    return 0;
}

// ============================================
// TEST: Utilities
// ============================================
int test_utilities(void) {
    TEST_START("Utilities");
    
    // Test timestamp
    uint64_t ts = utils_get_timestamp_ms();
    TEST_ASSERT(ts > 0, "Timestamp should be > 0");
    TEST_PASS("Timestamp working");
    
    // Test sleep
    uint64_t start = utils_get_timestamp_ms();
    utils_sleep_ms(100);
    uint64_t end = utils_get_timestamp_ms();
    uint64_t diff = end - start;
    TEST_ASSERT(diff >= 90 && diff <= 150, "Sleep inaccurate");
    TEST_PASS("Sleep working");
    
    // Test string operations
    char test_str[] = "  Hello World!  ";
    utils_string_trim(test_str);
    TEST_ASSERT(strcmp(test_str, "Hello World!") == 0, "String trim failed");
    TEST_PASS("String trim working");
    
    // Test file operations
    const char* test_file = "/tmp/test_utils.txt";
    const char* test_data = "Hello, World!";
    int result = utils_file_write_all(test_file, test_data, strlen(test_data));
    TEST_ASSERT(result == 0, "File write failed");
    TEST_PASS("File write working");
    
    bool exists = utils_file_exists(test_file);
    TEST_ASSERT(exists, "File should exist");
    TEST_PASS("File exists working");
    
    size_t size = utils_file_get_size(test_file);
    TEST_ASSERT(size == strlen(test_data), "File size mismatch");
    TEST_PASS("File size working");
    
    void* data = NULL;
    size_t data_size = 0;
    result = utils_file_read_all(test_file, &data, &data_size);
    TEST_ASSERT(result == 0, "File read failed");
    TEST_ASSERT(data_size == strlen(test_data), "Data size mismatch");
    TEST_ASSERT(memcmp(data, test_data, data_size) == 0, "Data mismatch");
    TEST_PASS("File read working");
    
    free(data);
    remove(test_file);
    
    return 0;
}

// ============================================
// TEST: Endurance (Long Duration Test)
// ============================================
int test_endurance(void) {
    TEST_START("Endurance Test (5 seconds)");
    
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.3f
    };
    strcpy(config.device_name, "default");
    
    audio_init(&config);
    
    // Create 5 second sine wave
    AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 5000);
    TEST_ASSERT(buffer != NULL, "Buffer creation failed");
    audio_fill_sine_wave(buffer, 440.0f, 0.3f);
    
    // Play for 5 seconds
    printf("🎵 Playing 440Hz for 5 seconds...\n");
    audio_play(buffer);
    
    // Let it play
    uint64_t start_time = utils_get_timestamp_ms();
    while ((utils_get_timestamp_ms() - start_time) < 5000 && test_running) {
        usleep(100000);
    }
    
    audio_stop();
    audio_free_buffer(buffer);
    audio_cleanup();
    
    TEST_PASS("Endurance test completed");
    return 0;
}

// ============================================
// RUN ALL TESTS
// ============================================
int main(int argc, char* argv[]) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   Audio Engine Test Suite              ║\n");
    printf("║   Verdin iMX8MP Audio Project          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    // Setup signal handler
    signal(SIGINT, test_signal_handler);
    signal(SIGTERM, test_signal_handler);
    
    // Set log level
    utils_set_log_level(LOG_LEVEL_INFO);
    
    // Run specific test if specified
    if (argc > 1) {
        if (strcmp(argv[1], "init") == 0) {
            return test_audio_init();
        } else if (strcmp(argv[1], "sine") == 0) {
            return test_sine_wave_generation();
        } else if (strcmp(argv[1], "wav") == 0) {
            return test_wav_operations();
        } else if (strcmp(argv[1], "buffer") == 0) {
            return test_buffer_operations();
        } else if (strcmp(argv[1], "volume") == 0) {
            return test_volume_control();
        } else if (strcmp(argv[1], "format") == 0) {
            return test_format_conversion();
        } else if (strcmp(argv[1], "performance") == 0) {
            return test_performance();
        } else if (strcmp(argv[1], "error") == 0) {
            return test_error_handling();
        } else if (strcmp(argv[1], "utils") == 0) {
            return test_utilities();
        } else if (strcmp(argv[1], "endurance") == 0) {
            return test_endurance();
        } else {
            printf("Unknown test: %s\n", argv[1]);
            printf("Available tests: init, sine, wav, buffer, volume, format, ");
            printf("performance, error, utils, endurance, all\n");
            return 1;
        }
    }
    
    // Run all tests
    int failed_tests = 0;
    
    failed_tests += test_utilities();
    failed_tests += test_buffer_operations();
    failed_tests += test_volume_control();
    failed_tests += test_format_conversion();
    failed_tests += test_wav_operations();
    failed_tests += test_audio_init();
    failed_tests += test_sine_wave_generation();
    failed_tests += test_error_handling();
    failed_tests += test_performance();
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   Test Results Summary                ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("✅ Passed: %d\n", tests_passed);
    printf("❌ Failed: %d\n", tests_failed);
    printf("📊 Total:  %d\n", tests_passed + tests_failed);
    printf("========================================\n");
    
    if (tests_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("\n⚠️ SOME TESTS FAILED!\n\n");
        return 1;
    }
}
