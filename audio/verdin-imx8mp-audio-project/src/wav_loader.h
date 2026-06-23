#ifndef WAV_LOADER_H
#define WAV_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "audio_engine.h"

typedef struct {
    uint32_t chunk_id;
    uint32_t chunk_size;
    uint32_t format;
    uint32_t subchunk1_id;
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t subchunk2_id;
    uint32_t subchunk2_size;
} WavHeader;

// WAV file operations
int wav_load_from_file(const char* filename, AudioBuffer** out_buffer);
int wav_save_to_file(const char* filename, AudioBuffer* buffer);
void wav_print_info(WavHeader* header);

// Utility functions
bool wav_validate_header(WavHeader* header);
uint32_t wav_get_data_size(WavHeader* header);

#endif // WAV_LOADER_H
