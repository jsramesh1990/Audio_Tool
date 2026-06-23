#include "wav_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// Convert little-endian to host
static uint32_t le_to_host32(uint32_t le_value) {
    return le_value;
    // On little-endian systems, just return as is
    // For portability, you might want to use htonl/ntohl
}

int wav_load_from_file(const char* filename, AudioBuffer** out_buffer) {
    FILE* file;
    WavHeader header;
    AudioBuffer* buffer;
    size_t bytes_read;
    
    if (!filename || !out_buffer) {
        return -1;
    }
    
    file = fopen(filename, "rb");
    if (!file) {
        return -1;
    }
    
    // Read header
    bytes_read = fread(&header, 1, sizeof(WavHeader), file);
    if (bytes_read != sizeof(WavHeader)) {
        fclose(file);
        return -1;
    }
    
    // Validate WAV header
    if (!wav_validate_header(&header)) {
        fclose(file);
        return -1;
    }
    
    // Create audio buffer
    buffer = (AudioBuffer*)calloc(1, sizeof(AudioBuffer));
    if (!buffer) {
        fclose(file);
        return -1;
    }
    
    buffer->sample_rate = header.sample_rate;
    buffer->channels = header.num_channels;
    buffer->bits_per_sample = header.bits_per_sample;
    buffer->data_size = header.subchunk2_size;
    buffer->buffer_size = header.subchunk2_size;
    
    // Allocate data buffer
    buffer->buffer = malloc(header.subchunk2_size);
    if (!buffer->buffer) {
        free(buffer);
        fclose(file);
        return -1;
    }
    
    // Read audio data
    bytes_read = fread(buffer->buffer, 1, header.subchunk2_size, file);
    if (bytes_read != header.subchunk2_size) {
        free(buffer->buffer);
        free(buffer);
        fclose(file);
        return -1;
    }
    
    fclose(file);
    *out_buffer = buffer;
    return 0;
}

int wav_save_to_file(const char* filename, AudioBuffer* buffer) {
    FILE* file;
    WavHeader header;
    uint32_t data_size;
    
    if (!filename || !buffer || !buffer->buffer) {
        return -1;
    }
    
    file = fopen(filename, "wb");
    if (!file) {
        return -1;
    }
    
    // Calculate sizes
    data_size = buffer->data_size;
    uint32_t bytes_per_sample = buffer->bits_per_sample / 8;
    uint32_t byte_rate = buffer->sample_rate * buffer->channels * bytes_per_sample;
    uint32_t block_align = buffer->channels * bytes_per_sample;
    
    // Prepare header
    header.chunk_id = 0x46464952; // "RIFF"
    header.chunk_size = 36 + data_size;
    header.format = 0x45564157; // "WAVE"
    header.subchunk1_id = 0x20746d66; // "fmt "
    header.subchunk1_size = 16;
    header.audio_format = 1; // PCM
    header.num_channels = buffer->channels;
    header.sample_rate = buffer->sample_rate;
    header.byte_rate = byte_rate;
    header.block_align = block_align;
    header.bits_per_sample = buffer->bits_per_sample;
    header.subchunk2_id = 0x61746164; // "data"
    header.subchunk2_size = data_size;
    
    // Write header
    fwrite(&header, 1, sizeof(WavHeader), file);
    
    // Write data
    fwrite(buffer->buffer, 1, data_size, file);
    
    fclose(file);
    return 0;
}

void wav_print_info(WavHeader* header) {
    printf("\n=== WAV File Info ===\n");
    printf("Channels: %d\n", header->num_channels);
    printf("Sample Rate: %d Hz\n", header->sample_rate);
    printf("Bits per Sample: %d\n", header->bits_per_sample);
    printf("Data Size: %d bytes\n", header->subchunk2_size);
    printf("Duration: %.2f seconds\n", 
           (float)header->subchunk2_size / 
           (header->sample_rate * header->num_channels * 
            header->bits_per_sample / 8));
    printf("=====================\n\n");
}

bool wav_validate_header(WavHeader* header) {
    // Check RIFF header
    if (header->chunk_id != 0x46464952) { // "RIFF"
        return false;
    }
    
    // Check WAVE format
    if (header->format != 0x45564157) { // "WAVE"
        return false;
    }
    
    // Check fmt subchunk
    if (header->subchunk1_id != 0x20746d66) { // "fmt "
        return false;
    }
    
    // Check PCM format
    if (header->audio_format != 1) {
        return false;
    }
    
    // Validate parameters
    if (header->num_channels < 1 || header->num_channels > 8) {
        return false;
    }
    
    if (header->sample_rate < 8000 || header->sample_rate > 192000) {
        return false;
    }
    
    if (header->bits_per_sample != 16 && 
        header->bits_per_sample != 24 && 
        header->bits_per_sample != 32) {
        return false;
    }
    
    // Check data subchunk
    if (header->subchunk2_id != 0x61746164) { // "data"
        return false;
    }
    
    return true;
}

uint32_t wav_get_data_size(WavHeader* header) {
    return header->subchunk2_size;
}
