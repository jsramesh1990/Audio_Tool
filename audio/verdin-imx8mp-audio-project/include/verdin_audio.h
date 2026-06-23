// Simple example using the main header
#include "verdin_audio.h"

int main() {
    // Configuration
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.8f
    };
    strcpy(config.device_name, "default");
    
    // Initialize
    if (verdin_audio_init(&config) != 0) {
        printf("Error: %s\n", audio_get_error());
        return -1;
    }
    
    // Get project info
    char info[256];
    verdin_audio_get_info(info, sizeof(info));
    printf("%s\n", info);
    
    // Create and play sine wave
    AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 3000);
    audio_fill_sine_wave(buffer, 440.0f, 0.5f);
    audio_play(buffer);
    
    // Wait
    sleep(3);
    
    // Cleanup
    audio_stop();
    audio_free_buffer(buffer);
    verdin_audio_cleanup();
    
    return 0;
}
