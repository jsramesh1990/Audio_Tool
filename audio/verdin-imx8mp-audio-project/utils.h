#include "utils.h"

// Initialize logging
utils_set_log_level(LOG_LEVEL_DEBUG);
utils_set_log_file("/var/log/audio_app.log");

// Log messages
LOG_INFO("Audio app started");
LOG_DEBUG("Buffer size: %d bytes", buffer_size);

// Generate a sine wave with volume control
float* audio_data = (float*)utils_malloc_safe(num_samples * sizeof(float));
// ... fill with audio data ...
utils_apply_volume(audio_data, num_samples, AUDIO_FORMAT_FLOAT, 0.8f);

// Check CPU usage
int cpu = utils_get_cpu_usage();
LOG_INFO("CPU usage: %d%%", cpu);
