#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "audio_engine.h"
#include "wav_loader.h"
#include "audio_config.h"

static volatile int running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nShutting down audio application...\n");
        running = 0;
        audio_stop();
    }
}

void print_usage(const char* prog_name) {
    printf("Usage: %s [OPTIONS]\n", prog_name);
    printf("Options:\n");
    printf("  -p <file.wav>   Play WAV file\n");
    printf("  -r <duration>   Record for X seconds\n");
    printf("  -s <freq>       Generate and play sine wave at freq Hz\n");
    printf("  -l              List available audio devices\n");
    printf("  -v <volume>     Set volume (0.0 to 1.0)\n");
    printf("  -d <device>     Audio device name (default: default)\n");
    printf("  -h              Show this help\n");
    printf("\nExamples:\n");
    printf("  %s -p test.wav\n", prog_name);
    printf("  %s -s 440\n", prog_name);
    printf("  %s -r 5\n", prog_name);
    printf("  %s -l\n", prog_name);
}

void list_audio_devices(void) {
    int card = -1;
    snd_ctl_t* ctl;
    snd_pcm_info_t* info;
    char name[64];
    
    snd_pcm_info_malloc(&info);
    
    while (1) {
        card++;
        snprintf(name, sizeof(name), "hw:%d", card);
        if (snd_ctl_open(&ctl, name, 0) < 0) {
            break;
        }
        
        if (snd_ctl_pcm_info(ctl, 0, info) >= 0) {
            printf("Card %d: %s\n", card, name);
        }
        
        snd_ctl_close(ctl);
    }
    
    snd_pcm_info_free(info);
}

int main(int argc, char* argv[]) {
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.8f,
        .is_playing = false,
        .is_recording = false
    };
    
    AudioBuffer* buffer = NULL;
    int opt;
    int result = 0;
    bool play_mode = false;
    bool generate_mode = false;
    float freq = 440.0f;
    int record_seconds = 0;
    
    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Default device name
    strcpy(config.device_name, "default");
    
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "hp:r:s:lv:d:")) != -1) {
        switch(opt) {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'l':
                list_audio_devices();
                return 0;
            case 'p':
                // Load and play WAV file
                if (wav_load_from_file(optarg, &buffer) != 0) {
                    fprintf(stderr, "Error loading WAV file: %s\n", optarg);
                    return 1;
                }
                play_mode = true;
                break;
            case 'r':
                record_seconds = atoi(optarg);
                if (record_seconds <= 0) {
                    fprintf(stderr, "Invalid record duration\n");
                    return 1;
                }
                break;
            case 's':
                freq = atof(optarg);
                if (freq <= 0 || freq > 20000) {
                    fprintf(stderr, "Invalid frequency (must be 1-20000 Hz)\n");
                    return 1;
                }
                generate_mode = true;
                break;
            case 'v':
                config.volume = atof(optarg);
                if (config.volume < 0 || config.volume > 1.0f) {
                    fprintf(stderr, "Volume must be between 0.0 and 1.0\n");
                    return 1;
                }
                break;
            case 'd':
                strncpy(config.device_name, optarg, sizeof(config.device_name) - 1);
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Check if no action specified
    if (!play_mode && !generate_mode && record_seconds == 0) {
        fprintf(stderr, "No action specified. Use -h for help.\n");
        return 1;
    }
    
    // Initialize audio
    if (audio_init(&config) != 0) {
        fprintf(stderr, "Failed to initialize audio: %s\n", audio_get_error());
        return 1;
    }
    
    printf("Audio initialized successfully\n");
    printf("Device: %s, Sample Rate: %d, Channels: %d, Bits: %d\n",
           config.device_name, config.sample_rate, 
           config.channels, config.bits_per_sample);
    
    // Set volume
    audio_set_volume(config.volume);
    
    // Handle playback mode
    if (play_mode && buffer) {
        printf("Playing: %s\n", "WAV file");
        if (audio_play(buffer) != 0) {
            fprintf(stderr, "Playback error: %s\n", audio_get_error());
            result = 1;
            goto cleanup;
        }
        
        printf("Playing... Press Ctrl+C to stop\n");
        while (running && config.is_playing) {
            usleep(100000); // 100ms
        }
        audio_stop();
    }
    
    // Handle sine wave generation
    if (generate_mode) {
        buffer = audio_create_buffer(config.sample_rate, 
                                     config.channels, 
                                     config.bits_per_sample, 
                                     5000); // 5 seconds
        if (!buffer) {
            fprintf(stderr, "Failed to create buffer\n");
            result = 1;
            goto cleanup;
        }
        
        if (audio_fill_sine_wave(buffer, freq, 0.5f) != 0) {
            fprintf(stderr, "Failed to generate sine wave\n");
            result = 1;
            goto cleanup;
        }
        
        printf("Generating sine wave at %.1f Hz\n", freq);
        if (audio_play(buffer) != 0) {
            fprintf(stderr, "Playback error: %s\n", audio_get_error());
            result = 1;
            goto cleanup;
        }
        
        printf("Playing... Press Ctrl+C to stop\n");
        while (running && config.is_playing) {
            usleep(100000);
        }
        audio_stop();
    }
    
    // Handle recording mode
    if (record_seconds > 0) {
        printf("Recording for %d seconds...\n", record_seconds);
        // TODO: Implement recording
        // For now, just sleep
        sleep(record_seconds);
    }
    
cleanup:
    // Clean up
    if (buffer) {
        audio_free_buffer(buffer);
    }
    audio_cleanup();
    
    printf("Audio application terminated\n");
    return result;
}

// Additional utility functions implementation
AudioBuffer* audio_create_buffer(uint32_t sample_rate, uint8_t channels, 
                                  uint8_t bits, uint32_t duration_ms) {
    AudioBuffer* buffer = (AudioBuffer*)calloc(1, sizeof(AudioBuffer));
    if (!buffer) return NULL;
    
    uint32_t bytes_per_sample = bits / 8;
    uint32_t num_samples = (sample_rate * duration_ms) / 1000;
    buffer->data_size = num_samples * channels * bytes_per_sample;
    buffer->buffer_size = buffer->data_size;
    buffer->sample_rate = sample_rate;
    buffer->channels = channels;
    buffer->bits_per_sample = bits;
    
    buffer->buffer = calloc(1, buffer->data_size);
    if (!buffer->buffer) {
        free(buffer);
        return NULL;
    }
    
    return buffer;
}

void audio_free_buffer(AudioBuffer* buffer) {
    if (buffer) {
        if (buffer->buffer) {
            free(buffer->buffer);
        }
        free(buffer);
    }
}
