/**
 * C program to generate test WAV files without external dependencies
 * Compile: gcc -o generate_wav generate_wav.c -lm
 * Run: ./generate_wav
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

// WAV file writing functions
void write_wav_header(FILE* file, int sample_rate, int channels, int bits, int data_size) {
    uint32_t chunk_size = 36 + data_size;
    uint32_t byte_rate = sample_rate * channels * (bits / 8);
    uint16_t block_align = channels * (bits / 8);
    
    // RIFF header
    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    
    // fmt subchunk
    fwrite("fmt ", 1, 4, file);
    uint32_t fmt_size = 16;
    fwrite(&fmt_size, 4, 1, file);
    uint16_t audio_format = 1; // PCM
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits, 2, 1, file);
    
    // data subchunk
    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);
}

void generate_sine_wav(const char* filename, double frequency, int duration_sec, 
                       int sample_rate, double amplitude) {
    int num_samples = sample_rate * duration_sec;
    int data_size = num_samples * 2; // 16-bit
    FILE* file = fopen(filename, "wb");
    
    if (!file) {
        printf("Error: Cannot create %s\n", filename);
        return;
    }
    
    write_wav_header(file, sample_rate, 1, 16, data_size);
    
    // Generate and write samples
    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / sample_rate;
        double value = amplitude * sin(2.0 * M_PI * frequency * t);
        int16_t sample = (int16_t)(value * 32767);
        fwrite(&sample, 2, 1, file);
    }
    
    fclose(file);
    printf("✅ Generated: %s\n", filename);
}

void generate_silence_wav(const char* filename, int duration_sec, int sample_rate) {
    int num_samples = sample_rate * duration_sec;
    int data_size = num_samples * 2;
    FILE* file = fopen(filename, "wb");
    
    if (!file) {
        printf("Error: Cannot create %s\n", filename);
        return;
    }
    
    write_wav_header(file, sample_rate, 1, 16, data_size);
    
    // Write silence
    int16_t sample = 0;
    for (int i = 0; i < num_samples; i++) {
        fwrite(&sample, 2, 1, file);
    }
    
    fclose(file);
    printf("✅ Generated: %s\n", filename);
}

void generate_stereo_sine_wav(const char* filename, double freq_left, double freq_right,
                              int duration_sec, int sample_rate) {
    int num_samples = sample_rate * duration_sec;
    int data_size = num_samples * 4; // 2 channels * 16-bit
    FILE* file = fopen(filename, "wb");
    
    if (!file) {
        printf("Error: Cannot create %s\n", filename);
        return;
    }
    
    write_wav_header(file, sample_rate, 2, 16, data_size);
    
    // Generate and write samples
    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / sample_rate;
        int16_t left = (int16_t)(0.5 * sin(2.0 * M_PI * freq_left * t) * 32767);
        int16_t right = (int16_t)(0.5 * sin(2.0 * M_PI * freq_right * t) * 32767);
        fwrite(&left, 2, 1, file);
        fwrite(&right, 2, 1, file);
    }
    
    fclose(file);
    printf("✅ Generated: %s\n", filename);
}

int main() {
    printf("🎵 Generating test WAV files in C\n\n");
    
    // Generate basic sine waves
    generate_sine_wav("test_440hz.wav", 440.0, 3, 48000, 0.5);
    generate_sine_wav("test_1000hz.wav", 1000.0, 2, 48000, 0.5);
    generate_sine_wav("test_50hz.wav", 50.0, 3, 48000, 0.5);
    
    // Generate silence
    generate_silence_wav("test_silence.wav", 3, 48000);
    
    // Generate stereo
    generate_stereo_sine_wav("test_stereo.wav", 440.0, 880.0, 3, 48000);
    
    printf("\n✅ All test WAV files generated successfully!\n");
    printf("📁 Files:\n");
    printf("   - test_440hz.wav\n");
    printf("   - test_1000hz.wav\n");
    printf("   - test_50hz.wav\n");
    printf("   - test_silence.wav\n");
    printf("   - test_stereo.wav\n");
    
    return 0;
}
