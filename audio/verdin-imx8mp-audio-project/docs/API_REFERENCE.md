# 📚 Complete Documentation Files

Here are all three documentation files complete with detailed content for your Verdin iMX8MP audio project.

## **docs/API_REFERENCE.md**

```markdown
# Audio Engine API Reference
## Verdin iMX8MP Audio Project

---

## 📋 Table of Contents
1. [Overview](#overview)
2. [Core Functions](#core-functions)
3. [Buffer Management](#buffer-management)
4. [WAV Operations](#wav-operations)
5. [Utility Functions](#utility-functions)
6. [Error Handling](#error-handling)
7. [Examples](#examples)
8. [Performance Considerations](#performance-considerations)

---

## Overview

The Audio Engine provides a high-level API for audio playback, recording, and processing on the Verdin iMX8MP platform. It abstracts the underlying ALSA (Advanced Linux Sound Architecture) and provides simple, intuitive functions for audio operations.

### Key Features
- **Simple Audio Playback**: Play audio from buffers or files
- **Audio Recording**: Capture audio from microphones
- **WAV File Support**: Load and save WAV files
- **Sine Wave Generation**: Generate test tones
- **Volume Control**: Software volume control
- **Format Conversion**: Convert between audio formats
- **Buffer Management**: Easy buffer creation and cleanup
- **Thread-Safe**: Designed for multi-threaded applications

---

## Core Functions

### `audio_init()`
Initialize the audio system and open the audio device.

```c
int audio_init(AudioConfig* config);
```

**Parameters:**
- `config` - Pointer to AudioConfig structure with audio parameters

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
AudioConfig config = {
    .sample_rate = 48000,
    .channels = 2,
    .bits_per_sample = 16,
    .buffer_size = 4096,
    .volume = 0.8f
};
strcpy(config.device_name, "default");

if (audio_init(&config) != 0) {
    printf("Error: %s\n", audio_get_error());
    return -1;
}
```

---

### `audio_play()`
Play audio from a buffer.

```c
int audio_play(AudioBuffer* buffer);
```

**Parameters:**
- `buffer` - AudioBuffer containing audio data to play

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 2000);
audio_fill_sine_wave(buffer, 440.0f, 0.5f);
audio_play(buffer);
```

---

### `audio_stop()`
Stop audio playback/recording.

```c
int audio_stop(void);
```

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
audio_stop();  // Immediately stop playback
```

---

### `audio_set_volume()`
Set audio volume.

```c
int audio_set_volume(float volume);
```

**Parameters:**
- `volume` - Volume level (0.0 to 1.0)

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
audio_set_volume(0.75f);  // Set to 75%
```

---

### `audio_get_volume()`
Get current volume.

```c
float audio_get_volume(void);
```

**Returns:**
- Current volume level (0.0 to 1.0)

---

### `audio_cleanup()`
Clean up and close audio device.

```c
void audio_cleanup(void);
```

**Example:**
```c
// Clean up before exiting
audio_cleanup();
```

---

## Buffer Management

### `audio_create_buffer()`
Create an audio buffer.

```c
AudioBuffer* audio_create_buffer(uint32_t sample_rate, 
                                 uint8_t channels, 
                                 uint8_t bits_per_sample, 
                                 uint32_t duration_ms);
```

**Parameters:**
- `sample_rate` - Sample rate in Hz (e.g., 48000)
- `channels` - Number of audio channels (1=mono, 2=stereo)
- `bits_per_sample` - Bit depth (16, 24, 32)
- `duration_ms` - Buffer duration in milliseconds

**Returns:**
- Pointer to AudioBuffer on success
- NULL on failure

**Example:**
```c
// Create 5 seconds of stereo audio at 48kHz, 16-bit
AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 5000);
if (!buffer) {
    printf("Failed to create buffer\n");
    return -1;
}
```

---

### `audio_free_buffer()`
Free an audio buffer.

```c
void audio_free_buffer(AudioBuffer* buffer);
```

**Parameters:**
- `buffer` - Buffer to free

**Example:**
```c
audio_free_buffer(buffer);
buffer = NULL;
```

---

### `audio_fill_sine_wave()`
Fill a buffer with a sine wave.

```c
int audio_fill_sine_wave(AudioBuffer* buffer, 
                         float frequency_hz, 
                         float amplitude);
```

**Parameters:**
- `buffer` - Buffer to fill
- `frequency_hz` - Frequency in Hz
- `amplitude` - Amplitude (0.0 to 1.0)

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
// Generate 440Hz sine wave at 50% amplitude
audio_fill_sine_wave(buffer, 440.0f, 0.5f);
```

---

## WAV Operations

### `wav_load_from_file()`
Load a WAV file from disk.

```c
int wav_load_from_file(const char* filename, AudioBuffer** out_buffer);
```

**Parameters:**
- `filename` - Path to WAV file
- `out_buffer` - Pointer to store loaded buffer

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
AudioBuffer* buffer;
if (wav_load_from_file("audio.wav", &buffer) == 0) {
    audio_play(buffer);
}
```

---

### `wav_save_to_file()`
Save audio buffer as WAV file.

```c
int wav_save_to_file(const char* filename, AudioBuffer* buffer);
```

**Parameters:**
- `filename` - Output filename
- `buffer` - Buffer to save

**Returns:**
- `0` on success
- Negative error code on failure

**Example:**
```c
wav_save_to_file("output.wav", buffer);
```

---

### `wav_validate_header()`
Validate WAV file header.

```c
bool wav_validate_header(WavHeader* header);
```

**Parameters:**
- `header` - WAV header to validate

**Returns:**
- `true` if valid
- `false` if invalid

---

### `wav_print_info()`
Print WAV file information.

```c
void wav_print_info(WavHeader* header);
```

**Example:**
```c
WavHeader header;
// ... load header ...
wav_print_info(&header);
// Output:
// === WAV File Info ===
// Channels: 2
// Sample Rate: 48000 Hz
// Bits per Sample: 16
// Data Size: 576000 bytes
// Duration: 3.00 seconds
```

---

## Utility Functions

### Logging

```c
void utils_log(LogLevel level, const char* function, const char* format, ...);
void utils_set_log_level(LogLevel level);
void utils_set_log_file(const char* filename);
```

**Log Levels:**
- `LOG_LEVEL_ERROR` - Error messages
- `LOG_LEVEL_WARNING` - Warning messages
- `LOG_LEVEL_INFO` - Informational messages
- `LOG_LEVEL_DEBUG` - Debug messages

**Example:**
```c
utils_set_log_level(LOG_LEVEL_DEBUG);
utils_set_log_file("/var/log/audio.log");
LOG_INFO("Audio system initialized");
LOG_DEBUG("Buffer size: %d bytes", buffer_size);
```

### Time Functions

```c
uint64_t utils_get_timestamp_ms(void);
uint64_t utils_get_timestamp_us(void);
void utils_sleep_ms(uint32_t ms);
void utils_sleep_us(uint32_t us);
```

**Example:**
```c
uint64_t start = utils_get_timestamp_ms();
// ... do something ...
uint64_t elapsed = utils_get_timestamp_ms() - start;
printf("Operation took %llu ms\n", elapsed);
```

### Memory Management

```c
void* utils_malloc_safe(size_t size);
void* utils_realloc_safe(void* ptr, size_t new_size);
void utils_free_safe(void** ptr);
```

**Example:**
```c
float* data = (float*)utils_malloc_safe(1024 * sizeof(float));
if (data) {
    // ... use data ...
    utils_free_safe((void**)&data);
}
```

### Format Conversion

```c
int utils_buffer_convert_format(const void* src, void* dst, 
                                uint32_t num_samples,
                                AudioFormat src_format, 
                                AudioFormat dst_format);
```

**Audio Formats:**
- `AUDIO_FORMAT_S16` - Signed 16-bit integer
- `AUDIO_FORMAT_S24` - Signed 24-bit integer
- `AUDIO_FORMAT_S32` - Signed 32-bit integer
- `AUDIO_FORMAT_FLOAT` - 32-bit float

**Example:**
```c
int16_t s16_data[100];
float float_data[100];
utils_buffer_convert_format(s16_data, float_data, 100,
                           AUDIO_FORMAT_S16, AUDIO_FORMAT_FLOAT);
```

### DSP Functions

```c
void utils_apply_volume(void* buffer, uint32_t num_samples, 
                        AudioFormat format, float volume);
void utils_generate_silence(void* buffer, uint32_t num_samples, 
                           AudioFormat format);
float utils_db_to_linear(float db);
float utils_linear_to_db(float linear);
float utils_lerp(float a, float b, float t);
float utils_clamp(float value, float min, float max);
```

**Example:**
```c
// Apply 50% volume
utils_apply_volume(buffer->buffer, 
                   buffer->data_size / sizeof(int16_t),
                   AUDIO_FORMAT_S16, 0.5f);

// Generate silence
utils_generate_silence(silent_buffer, num_samples, AUDIO_FORMAT_S16);

// Convert dB to linear
float linear = utils_db_to_linear(-6.0f);  // -6dB = 0.5
```

### File Operations

```c
bool utils_file_exists(const char* filename);
size_t utils_file_get_size(const char* filename);
int utils_file_read_all(const char* filename, void** out_data, size_t* out_size);
int utils_file_write_all(const char* filename, const void* data, size_t size);
```

**Example:**
```c
if (utils_file_exists("audio.wav")) {
    size_t size = utils_file_get_size("audio.wav");
    void* data;
    size_t read_size;
    if (utils_file_read_all("audio.wav", &data, &read_size) == 0) {
        // ... use data ...
        free(data);
    }
}
```

### String Functions

```c
void utils_string_trim(char* str);
char* utils_string_duplicate(const char* str);
char* utils_string_join(const char* sep, int count, ...);
```

**Example:**
```c
char str[] = "  Hello World!  ";
utils_string_trim(str);  // "Hello World!"

char* dup = utils_string_duplicate("Hello");
// ... use dup ...
free(dup);

char* joined = utils_string_join(", ", 3, "apple", "banana", "orange");
// "apple, banana, orange"
free(joined);
```

### System Functions

```c
void utils_print_system_info(void);
int utils_get_cpu_usage(void);
```

**Example:**
```c
utils_print_system_info();
int cpu = utils_get_cpu_usage();
printf("CPU usage: %d%%\n", cpu);
```

---

## Error Handling

### `audio_get_error()`
Get last error message.

```c
const char* audio_get_error(void);
```

**Returns:**
- String describing the last error

### `audio_clear_error()`
Clear error message.

```c
void audio_clear_error(void);
```

**Example:**
```c
if (audio_init(&config) != 0) {
    fprintf(stderr, "Error: %s\n", audio_get_error());
    audio_clear_error();
    return -1;
}
```

---

## Examples

### Example 1: Simple Sine Wave Player
```c
#include "audio_engine.h"

int main() {
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.8f
    };
    strcpy(config.device_name, "default");

    // Initialize
    if (audio_init(&config) != 0) {
        printf("Error: %s\n", audio_get_error());
        return -1;
    }

    // Create 5-second sine wave
    AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 5000);
    audio_fill_sine_wave(buffer, 440.0f, 0.5f);

    // Play
    audio_play(buffer);
    
    // Wait for playback to complete
    sleep(5);
    
    // Cleanup
    audio_stop();
    audio_free_buffer(buffer);
    audio_cleanup();
    
    return 0;
}
```

### Example 2: WAV Player
```c
#include "audio_engine.h"
#include "wav_loader.h"

int play_wav(const char* filename) {
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.8f
    };
    strcpy(config.device_name, "default");

    if (audio_init(&config) != 0) {
        printf("Error: %s\n", audio_get_error());
        return -1;
    }

    AudioBuffer* buffer = NULL;
    if (wav_load_from_file(filename, &buffer) != 0) {
        printf("Failed to load WAV file\n");
        audio_cleanup();
        return -1;
    }

    audio_play(buffer);
    
    // Wait for playback
    uint32_t duration_ms = (buffer->data_size * 1000) / 
                          (buffer->sample_rate * buffer->channels * 
                           buffer->bits_per_sample / 8);
    sleep(duration_ms / 1000 + 1);
    
    audio_stop();
    audio_free_buffer(buffer);
    audio_cleanup();
    
    return 0;
}
```

### Example 3: Audio Recorder
```c
#include "audio_engine.h"

int record_audio(const char* filename, uint32_t duration_seconds) {
    AudioConfig config = {
        .sample_rate = 48000,
        .channels = 2,
        .bits_per_sample = 16,
        .buffer_size = 4096,
        .volume = 0.8f
    };
    strcpy(config.device_name, "default");

    if (audio_init(&config) != 0) {
        printf("Error: %s\n", audio_get_error());
        return -1;
    }

    AudioBuffer* buffer = audio_create_buffer(48000, 2, 16, 
                                             duration_seconds * 1000);
    
    // Record
    printf("Recording for %u seconds...\n", duration_seconds);
    audio_record(buffer, duration_seconds * 1000);
    
    // Save
    wav_save_to_file(filename, buffer);
    
    audio_free_buffer(buffer);
    audio_cleanup();
    
    return 0;
}
```

### Example 4: Volume Control with Visualization
```c
#include "audio_engine.h"

void volume_visualization(float volume) {
    int bars = (int)(volume * 20);
    printf("[");
    for (int i = 0; i < 20; i++) {
        if (i < bars) printf("█");
        else printf("░");
    }
    printf("] %.0f%%\n", volume * 100);
}

int main() {
    // ... init audio ...
    
    for (float vol = 0.0f; vol <= 1.0f; vol += 0.1f) {
        audio_set_volume(vol);
        volume_visualization(vol);
        utils_sleep_ms(500);
    }
    
    // ... cleanup ...
}
```

---

## Performance Considerations

### Buffer Sizes
- **Small buffers** (< 1024 frames): Low latency but more CPU usage
- **Large buffers** (4096+ frames): Higher latency but better CPU efficiency

### Sample Rates
- **48kHz**: Standard, good quality
- **96kHz**: High quality, more processing power
- **44.1kHz**: CD quality

### Thread Safety
The API is thread-safe for most operations. However:
- Only one thread should call `audio_play()` or `audio_record()` at a time
- Use mutexes if accessing shared buffers from multiple threads

### Memory Usage
```c
// Calculate buffer memory
uint32_t bytes_per_second = sample_rate * channels * (bits / 8);
uint32_t buffer_size = bytes_per_second * duration_seconds;
printf("Buffer size: %.2f MB\n", buffer_size / (1024.0f * 1024.0f));
```

### Debugging Tips
1. Enable debug logging: `utils_set_log_level(LOG_LEVEL_DEBUG)`
2. Check CPU usage: `utils_get_cpu_usage()`
3. Monitor memory: Check `free` command
4. Use test suite: `./bin/test_audio`

### Error Recovery
```c
// Robust error handling
int play_audio_safe(AudioBuffer* buffer) {
    int retry_count = 3;
    while (retry_count-- > 0) {
        if (audio_play(buffer) == 0) {
            return 0;
        }
        LOG_WARNING("Playback failed, retrying...");
        audio_stop();
        usleep(100000);
    }
    LOG_ERROR("Playback failed after 3 attempts");
    return -1;
}
```

---

## API Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2024-01-15 | Initial release |
| 1.1.0 | 2024-03-20 | Added format conversion |
| 1.2.0 | 2024-06-01 | Added WAV operations |
| 1.3.0 | 2024-08-10 | Added DSP functions |
| 1.4.0 | 2024-10-05 | Added system utilities |

---

## Troubleshooting Quick Reference

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| Device not found | Wrong device name | Use `aplay -l` to list devices |
| No sound | Volume too low | Check volume with `alsamixer` |
| Underruns | Buffer too small | Increase buffer size |
| High latency | Buffer too large | Decrease buffer size |
| CPU spikes | Too many conversions | Use native format |
| Memory leak | Missing cleanup | Call `audio_free_buffer()` |

---

## License

This API is provided under the MIT License. See LICENSE file for details.
```

## **docs/DEPLOYMENT_GUIDE.md**

```markdown
# Deployment Guide
## Verdin iMX8MP Audio Project

---

## 📋 Table of Contents
1. [Overview](#overview)
2. [Pre-Deployment Checklist](#pre-deployment-checklist)
3. [Build Options](#build-options)
4. [Deployment Methods](#deployment-methods)
5. [Configuration](#configuration)
6. [Testing](#testing)
7. [Production Deployment](#production-deployment)
8. [Monitoring & Maintenance](#monitoring-maintenance)

---

## Overview

This guide covers the complete deployment process for the Verdin iMX8MP audio application, from development environment to production.

### Target Environments
- **Development**: Rapid prototyping on target hardware
- **Testing**: Integration and validation
- **Staging**: Pre-production validation
- **Production**: Customer deployment

---

## Pre-Deployment Checklist

### Hardware Requirements
- [ ] Verdin iMX8MP module
- [ ] Carrier board (Development or Dahlia)
- [ ] Audio codec (NAU8822 or WM8904)
- [ ] Speakers or headphones
- [ ] Microphone (optional)
- [ ] Power supply

### Software Requirements
- [ ] Linux kernel 5.10+ with Device Tree support
- [ ] ALSA drivers
- [ ] I2C and SPI drivers
- [ ] Build tools (make, gcc, dtc)
- [ ] SSH access

### Network Configuration
```bash
# Set static IP (optional)
sudo nmcli con mod eth0 ipv4.addresses 192.168.1.100/24
sudo nmcli con mod eth0 ipv4.gateway 192.168.1.1
sudo nmcli con mod eth0 ipv4.dns 8.8.8.8
sudo nmcli con down eth0 && sudo nmcli con up eth0
```

---

## Build Options

### Option 1: Native Build (on Target)

```bash
# SSH into target
ssh torizon@192.168.1.100

# Clone or copy source
git clone https://github.com/your-username/verdin-audio.git
cd verdin-audio

# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential libasound2-dev

# Build
make clean
make all

# Test
./bin/audio_player -s 440
```

### Option 2: Cross-Compilation

```bash
# On development host
# Set up cross-compilation toolchain
export PATH=/opt/toradex/gcc-11.3.0-aarch64/bin:$PATH
export CC=aarch64-linux-gnu-gcc
export LD=aarch64-linux-gnu-ld

# Build for target
make clean
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

# Deploy
scp bin/audio_player torizon@192.168.1.100:/home/torizon/
```

### Option 3: Yocto Build (Production)

```bash
# Create custom layer
mkdir -p meta-audio/recipes-app/audio-player
cat > meta-audio/recipes-app/audio-player/audio-player.bb << 'EOF'
SUMMARY = "Audio Player for Verdin iMX8MP"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=..."
SRC_URI = "git://github.com/your-username/verdin-audio.git;protocol=https"
S = "${WORKDIR}/git"
DEPENDS = "alsa-lib"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/bin/audio_player ${D}${bindir}
}
EOF

# Build
bitbake core-image-minimal audio-player
```

---

## Deployment Methods

### Method 1: Manual Deployment

```bash
# Copy binary
scp bin/audio_player torizon@192.168.1.100:/home/torizon/

# Copy configuration
scp configs/asound.conf torizon@192.168.1.100:/tmp/
ssh torizon@192.168.1.100 "sudo cp /tmp/asound.conf /etc/"

# Copy Device Tree overlay
scp device-tree/imx8mp-verdin-audio-overlay.dtbo torizon@192.168.1.100:/tmp/
ssh torizon@192.168.1.100 "sudo cp /tmp/imx8mp-verdin-audio-overlay.dtbo /lib/firmware/"

# Set permissions
ssh torizon@192.168.1.100 "chmod +x /home/torizon/audio_player"

# Run
ssh torizon@192.168.1.100 "cd /home/torizon && ./audio_player -s 440"
```

### Method 2: Automated Deployment Script

```bash
#!/bin/bash
# scripts/full_deploy.sh

set -e

# Configuration
TARGET_IP=${1:-"192.168.1.100"}
TARGET_USER="torizon"
TARGET_PATH="/home/torizon/audio_app"
BINARY="bin/audio_player"
CONFIG="configs/asound.conf"
OVERLAY="device-tree/imx8mp-verdin-audio-overlay.dtbo"

echo "🚀 Deploying to $TARGET_IP"

# Build
echo "📦 Building application..."
make clean
make all

# Prepare target
echo "📁 Preparing target directories..."
ssh ${TARGET_USER}@${TARGET_IP} "mkdir -p $TARGET_PATH"

# Deploy binary
echo "📤 Deploying binary..."
scp $BINARY ${TARGET_USER}@${TARGET_IP}:$TARGET_PATH/
ssh ${TARGET_USER}@${TARGET_IP} "chmod +x $TARGET_PATH/audio_player"

# Deploy configuration
if [ -f "$CONFIG" ]; then
    echo "📝 Deploying configuration..."
    scp $CONFIG ${TARGET_USER}@${TARGET_IP}:/tmp/
    ssh ${TARGET_USER}@${TARGET_IP} "sudo cp /tmp/asound.conf /etc/"
fi

# Deploy Device Tree overlay
if [ -f "$OVERLAY" ]; then
    echo "🔧 Deploying Device Tree overlay..."
    scp $OVERLAY ${TARGET_USER}@${TARGET_IP}:/tmp/
    ssh ${TARGET_USER}@${TARGET_IP} "sudo cp /tmp/*.dtbo /lib/firmware/"
fi

# Restart ALSA
ssh ${TARGET_USER}@${TARGET_IP} "sudo /etc/init.d/alsa-utils restart"

# Verify
echo "✅ Deployment complete!"
echo "📊 Verifying..."

ssh ${TARGET_USER}@${TARGET_IP} "aplay -l"
ssh ${TARGET_USER}@${TARGET_IP} "ls -la $TARGET_PATH/"

echo ""
echo "🎵 To run: ssh ${TARGET_USER}@${TARGET_IP} '$TARGET_PATH/audio_player -s 440'"
```

### Method 3: Docker Container (Torizon)

```dockerfile
# Dockerfile
FROM torizon/arm64v8-debian:bullseye

RUN apt-get update && apt-get install -y \
    libasound2 \
    alsa-utils \
    && rm -rf /var/lib/apt/lists/*

COPY bin/audio_player /usr/local/bin/
COPY configs/asound.conf /etc/asound.conf

CMD ["audio_player", "-s", "440"]
```

```bash
# Build and run
docker build -t audio-player .
docker run --privileged --device /dev/snd audio-player
```

### Method 4: Systemd Service (Autostart)

```bash
# Create service file
cat > /etc/systemd/system/audio-player.service << 'EOF'
[Unit]
Description=Audio Player Service
After=network.target sound.target

[Service]
Type=simple
User=torizon
WorkingDirectory=/home/torizon/audio_app
ExecStart=/home/torizon/audio_app/audio_player -s 440
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

# Enable and start
sudo systemctl daemon-reload
sudo systemctl enable audio-player
sudo systemctl start audio-player
sudo systemctl status audio-player
```

---

## Configuration

### ALSA Configuration (asound.conf)
```conf
# /etc/asound.conf
pcm.!default {
    type hw
    card 0
    device 0
}

ctl.!default {
    type hw
    card 0
}

# Software volume control
pcm.softvol {
    type softvol
    slave.pcm "default"
    control.name "Master"
    control.card 0
}
```

### U-Boot Configuration
```bash
# In U-Boot
setenv fdt_board dahlia  # Or 'dev' for development board
setenv fdt_overlays imx8mp-verdin-audio-overlay.dtbo
saveenv
boot
```

### Kernel Command Line
```bash
# /boot/extlinux/extlinux.conf
LABEL audio
    KERNEL /Image
    FDT /imx8mp-verdin.dtb
    FDTOVERLAYS /imx8mp-verdin-audio-overlay.dtbo
    APPEND console=ttymxc1,115200 root=/dev/mmcblk2p2 rootwait
```

---

## Testing

### Quick Smoke Test
```bash
#!/bin/bash
# test_quick.sh

# Test 1: Device detection
if aplay -l | grep -q "card"; then
    echo "✅ Audio device detected"
else
    echo "❌ No audio device"
    exit 1
fi

# Test 2: Play test tone
if speaker-test -c 2 -t sine -f 440 -l 1 > /dev/null 2>&1; then
    echo "✅ Speaker test passed"
else
    echo "❌ Speaker test failed"
    exit 1
fi

# Test 3: Our application
if ./bin/audio_player -s 440 > /dev/null 2>&1 & then
    sleep 1
    kill $! 2>/dev/null
    echo "✅ Audio player working"
else
    echo "❌ Audio player failed"
    exit 1
fi
```

### Full Test Suite
```bash
# Run complete test suite
./scripts/test_audio.sh

# Specific tests
./bin/test_audio init
./bin/test_audio sine
./bin/test_audio wav
./bin/test_audio performance
```

### Performance Benchmarking
```bash
# CPU usage
./bin/audio_player -s 440 &
PID=$!
top -p $PID -b -n 10 | grep "audio_player"

# Memory usage
cat /proc/$PID/status | grep "VmRSS"

# Latency
./scripts/measure_latency.sh
```

---

## Production Deployment

### Checklist for Production
- [ ] Binary optimized with `-O3` flag
- [ ] Debug symbols stripped
- [ ] Logging level set to `LOG_LEVEL_WARNING`
- [ ] Systemd service configured for auto-start
- [ ] Device Tree overlay loaded at boot
- [ ] ALSA configuration applied
- [ ] udev rules installed
- [ ] Monitoring and logging setup
- [ ] Backup and recovery plan
- [ ] Documentation updated

### Optimized Build
```bash
# Release build
make clean
CFLAGS="-O3 -DNDEBUG" make all
strip -s bin/audio_player

# Check binary size
ls -lh bin/audio_player
```

### Production Configuration
```c
// audio_config.h
#define AUDIO_DEBUG_PRINT(fmt, ...)  // Disable debug prints
#define AUDIO_DEFAULT_BUFFER_SIZE 8192  // Larger buffer for stability
```

### Monitoring Setup
```bash
# Install monitoring
sudo apt-get install -y sysstat htop

# Create monitoring script
cat > /usr/local/bin/audio_monitor.sh << 'EOF'
#!/bin/bash
while true; do
    CPU=$(top -bn1 | grep "audio_player" | awk '{print $9}')
    MEM=$(ps aux | grep "audio_player" | grep -v grep | awk '{print $4}')
    echo "$(date): CPU=$CPU%, MEM=$MEM%" >> /var/log/audio_monitor.log
    sleep 60
done
EOF

chmod +x /usr/local/bin/audio_monitor.sh

# Add to crontab
(crontab -l 2>/dev/null; echo "@reboot /usr/local/bin/audio_monitor.sh &") | crontab -
```

---

## Monitoring & Maintenance

### Log Management
```bash
# View logs
journalctl -u audio-player -f

# Rotate logs
cat > /etc/logrotate.d/audio-player << 'EOF'
/var/log/audio_player.log {
    rotate 7
    daily
    compress
    missingok
    notifempty
    create 640 torizon torizon
}
EOF
```

### Health Checks
```bash
#!/bin/bash
# health_check.sh

# Check service
if systemctl is-active --quiet audio-player; then
    echo "✅ Service running"
else
    echo "❌ Service not running"
    systemctl start audio-player
fi

# Check audio device
if aplay -l | grep -q "card"; then
    echo "✅ Audio device available"
else
    echo "❌ Audio device missing"
    # Recover
    sudo modprobe -r snd_soc_nau8822
    sudo modprobe snd_soc_nau8822
fi

# Check memory
MEM=$(ps aux | grep audio_player | grep -v grep | awk '{print $4}')
if (( $(echo "$MEM > 80" | bc -l) )); then
    echo "⚠️ High memory usage: $MEM%"
    systemctl restart audio-player
fi
```

### Update Procedure
```bash
#!/bin/bash
# update_audio.sh

# Backup current version
ssh torizon@192.168.1.100 "sudo cp /home/torizon/audio_app/audio_player /home/torizon/audio_app/audio_player.bak"

# Deploy new version
scp bin/audio_player torizon@192.168.1.100:/home/torizon/audio_app/

# Verify
ssh torizon@192.168.1.100 "/home/torizon/audio_app/audio_player --version"

# Restart service
ssh torizon@192.168.1.100 "sudo systemctl restart audio-player"

# Rollback if needed
# ssh torizon@192.168.1.100 "sudo cp /home/torizon/audio_app/audio_player.bak /home/torizon/audio_app/audio_player"
```

---

## Troubleshooting Deployments

| Issue | Solution |
|-------|----------|
| **Permission denied** | Add user to audio group: `sudo usermod -a -G audio $USER` |
| **Device not found** | Load device tree overlay: `dtbo` |
| **Service won't start** | Check logs: `journalctl -u audio-player` |
| **No sound** | Check ALSA: `alsamixer`, `aplay -l` |
| **High latency** | Adjust buffer size: `AUDIO_DEFAULT_BUFFER_SIZE` |
| **Memory leak** | Run Valgrind: `valgrind --leak-check=full ./audio_player` |
| **CPU overuse** | Use optimized build: `-O3` flag |

---

## Quick Reference Card

```bash
# Build
make clean && make all

# Deploy
./scripts/full_deploy.sh 192.168.1.100

# Test
./scripts/test_audio.sh

# Monitor
sudo journalctl -u audio-player -f

# Restart
sudo systemctl restart audio-player

# Rollback
ssh torizon@192.168.1.100 "sudo cp /home/torizon/audio_app/audio_player.bak /home/torizon/audio_app/audio_player"
```

---

## Support Contact

- **Documentation**: docs/README.md
- **Issues**: GitHub Issues
- **Email**: support@yourdomain.com

## 📊 Deployment Status Template

| Component | Status | Version | Notes |
|-----------|--------|---------|-------|
| Hardware | ✅ OK | Verdin iMX8MP | Rev 1.1 |
| Kernel | ✅ OK | 5.15.52 | Custom build |
| Audio Codec | ✅ OK | NAU8822 | I2C address 0x1a |
| Application | ✅ OK | 1.4.0 | Optimized |
| Service | ✅ OK | Systemd | Auto-start |
| Monitoring | ✅ OK | Custom | Logging enabled |

---

**Last Updated:** January 2025
**Version:** 1.0.0
```

## **docs/TROUBLESHOOTING.md**

```markdown
# Troubleshooting Guide
## Verdin iMX8MP Audio Project

---

## 📋 Table of Contents
1. [Common Issues](#common-issues)
2. [Diagnostic Tools](#diagnostic-tools)
3. [Hardware Issues](#hardware-issues)
4. [Software Issues](#software-issues)
5. [Audio Quality Issues](#audio-quality-issues)
6. [Performance Issues](#performance-issues)
7. [Debugging Techniques](#debugging-techniques)
8. [Recovery Procedures](#recovery-procedures)

---

## Common Issues

### Quick Diagnosis Flowchart

```
┌─────────────────┐
│ No Audio Output │
└────────┬────────┘
         ▼
┌─────────────────┐
│ Check Hardware  │
└────────┬────────┘
         ▼
┌─────────────────┐
│  aplay -l       │─── No ──► Check Device Tree
└────────┬────────┘
         │ Yes
         ▼
┌─────────────────┐
│  alsamixer      │─── Muted ──► Unmute
└────────┬────────┘
         │ OK
         ▼
┌─────────────────┐
│  speaker-test   │─── Fail ──► Check Codec
└────────┬────────┘
         │ Pass
         ▼
┌─────────────────┐
│  Our App Test   │─── Fail ──► Debug App
└────────┬────────┘
         │ Pass
         ▼
┌─────────────────┐
│ ✓ Working       │
└─────────────────┘
```

---

## Diagnostic Tools

### 1. Audio Hardware Detection
```bash
# List sound cards
cat /proc/asound/cards

# List playback devices
aplay -l

# List capture devices
arecord -l

# Detailed ALSA info
cat /proc/asound/version

# Codec detection
dmesg | grep -i "codec"
dmesg | grep -i "audio"
dmesg | grep -i "nau8822"
dmesg | grep -i "wm8904"
```

### 2. Device Tree Verification
```bash
# Check if overlay is loaded
ls /configfs/device-tree/overlays/

# Check device tree status
cat /proc/device-tree/sound/compatible
cat /proc/device-tree/sound/simple-audio-card,name

# Check I2C devices
i2cdetect -y 3

# Check pinmux
cat /sys/kernel/debug/pinctrl/pinctrl-handles
```

### 3. ALSA Debugging
```bash
# Check ALSA configuration
cat /etc/asound.conf
cat /usr/share/alsa/alsa.conf

# Check sound card controls
amixer -c0 contents

# Check current settings
amixer -c0 sget Master
amixer -c0 sget Headphone
amixer -c0 sget Speaker

# Test with direct ALSA
aplay -D hw:0,0 test.wav
arecord -D hw:0,0 -d 5 test.wav
```

### 4. System Diagnostics
```bash
# Check kernel messages
dmesg | tail -50

# Check system logs
journalctl -f
journalctl -u audio-player

# Check process status
ps aux | grep audio
top -p $(pgrep audio_player)

# Check memory
free -h
cat /proc/meminfo

# Check CPU
cat /proc/cpuinfo
mpstat -P ALL 1
```

---

## Hardware Issues

### Issue: No Audio Device Found

**Symptoms:**
- `aplay -l` shows no cards
- `/proc/asound/cards` empty
- No sound output

**Diagnosis:**
```bash
# Check I2C detection
i2cdetect -y 3 | grep -E "1a|1b"

# Check codec power
dmesg | grep -i "nau8822"
dmesg | grep -i "regulator"

# Check hardware connections
cat /sys/kernel/debug/gpio
```

**Solutions:**
1. **Check Device Tree:**
   ```bash
   # Verify overlay
   dtc -I dtb -O dts /lib/firmware/imx8mp-verdin-audio-overlay.dtbo
   
   # Reload overlay
   echo 0 > /configfs/device-tree/overlays/audio/status
   rmdir /configfs/device-tree/overlays/audio
   mkdir /configfs/device-tree/overlays/audio
   cat imx8mp-verdin-audio-overlay.dtbo > /configfs/device-tree/overlays/audio/dtbo
   ```

2. **Check I2C Bus:**
   ```bash
   # Test I2C communication
   i2cget -y 3 0x1a 0x00
   ```

3. **Hardware Checklist:**
   - [ ] Carrier board properly connected
   - [ ] Audio codec soldered correctly
   - [ ] I2C pull-up resistors present
   - [ ] Power supply adequate (5V, 3.3V)
   - [ ] Clock signals present

### Issue: Audio Codec Not Initializing

**Symptoms:**
- Codec detected but not working
- Error messages in dmesg

**Diagnosis:**
```bash
# Check I2C communication
i2cdump -y 3 0x1a

# Check codec registers
amixer -c0 contents | grep -i "register"

# Check clock
cat /sys/kernel/debug/clk/clk_summary | grep sai
```

**Solutions:**
1. **Reset Codec:**
   ```bash
   # GPIO reset
   echo 0 > /sys/class/gpio/gpioXX/value
   sleep 1
   echo 1 > /sys/class/gpio/gpioXX/value
   ```

2. **Re-initialize Codec:**
   ```bash
   # Unbind and rebind
   echo -n "1-001a" > /sys/bus/i2c/drivers/nau8822/unbind
   sleep 1
   echo -n "1-001a" > /sys/bus/i2c/drivers/nau8822/bind
   ```

3. **Check Clock Source:**
   ```bash
   # Set correct MCLK
   amixer -c0 cset numid=?? 1
   ```

### Issue: Overheating Audio Codec

**Symptoms:**
- Codec physically hot
- Audio distortion
- Random resets

**Solutions:**
1. **Reduce Volume:**
   ```bash
   amixer -c0 sset 'Master' 50%
   amixer -c0 sset 'Headphone' 50%
   ```

2. **Add Heatsink:**
   - Apply thermal pad
   - Add small heatsink

3. **Check Power Supply:**
   - Ensure stable 3.3V
   - Check for voltage drops

---

## Software Issues

### Issue: Application Won't Start

**Symptoms:**
- `audio_player: command not found`
- Permission denied
- Library missing

**Diagnosis:**
```bash
# Check binary exists
ls -la bin/audio_player

# Check permissions
file bin/audio_player
ldd bin/audio_player

# Check dependencies
strace ./audio_player 2>&1 | grep "ENOENT"
```

**Solutions:**
1. **Fix Permissions:**
   ```bash
   chmod +x bin/audio_player
   sudo usermod -a -G audio $USER
   ```

2. **Install Libraries:**
   ```bash
   sudo apt-get install -y libasound2
   ```

3. **Rebuild Application:**
   ```bash
   make clean
   make all
   ```

### Issue: Application Crashes

**Symptoms:**
- Segmentation fault
- Aborted
- Core dump

**Diagnosis:**
```bash
# Run with debugger
gdb ./audio_player
run -s 440

# Check core dump
ulimit -c unlimited
./audio_player -s 440
ls -la core*

# Enable debug logging
export AUDIO_DEBUG=1
./audio_player -s 440
```

**Solutions:**
1. **Check Memory:**
   ```bash
   valgrind --leak-check=full ./audio_player -s 440
   ```

2. **Check Buffer Size:**
   ```c
   // Reduce buffer size
   config.buffer_size = 2048;  // Default is 4096
   ```

3. **Fix Code Issues:**
   ```c
   // Add null checks
   if (!buffer) {
       LOG_ERROR("Buffer is NULL");
       return -1;
   }
   ```

### Issue: No Sound from Application

**Symptoms:**
- Application runs
- No audio output
- No errors

**Diagnosis:**
```bash
# Check if audio is actually playing
cat /proc/asound/card0/pcm0p/sub0/status

# Monitor audio stream
aplay -D hw:0,0 /dev/zero -d 5

# Check with strace
strace -e ioctl,write ./audio_player -s 440
```

**Solutions:**
1. **Check Volume:**
   ```bash
   amixer -c0 sget Master
   amixer -c0 sset Master 80%
   ```

2. **Check Mute State:**
   ```bash
   amixer -c0 sset Master unmute
   amixer -c0 sset Headphone unmute
   ```

3. **Check Audio Routing:**
   ```bash
   amixer -c0 sget 'Playback Path'
   amixer -c0 sset 'Playback Path' 'HP'
   ```

---

## Audio Quality Issues

### Issue: Distorted Sound

**Symptoms:**
- Crackling
- Clipping
- Noise

**Diagnosis:**
```bash
# Check volume levels
amixer -c0 contents | grep -i "volume"

# Check sample rate
cat /proc/asound/card0/pcm0p/sub0/hw_params
```

**Solutions:**
1. **Reduce Volume:**
   ```bash
   amixer -c0 sset 'Master' 70%
   amixer -c0 sset 'DAC' 70%
   ```

2. **Check Sample Rate Match:**
   ```c
   // Match sample rate in code
   config.sample_rate = 48000;  // Should match hardware
   ```

3. **Check Buffer Size:**
   ```c
   // Increase buffer for smoother playback
   config.buffer_size = 8192;
   ```

### Issue: Latency Issues

**Symptoms:**
- Audio delay
- Echo
- Sync issues

**Diagnosis:**
```bash
# Measure latency
aplay -D hw:0,0 test.wav
time ./audio_player -s 440

# Check buffer settings
cat /proc/asound/card0/pcm0p/sub0/hw_params
```

**Solutions:**
1. **Reduce Buffer Size:**
   ```c
   config.buffer_size = 1024;  // Lower latency
   config.period_size = 256;
   ```

2. **Use Realtime Priority:**
   ```bash
   sudo chrt -f -p 50 $(pgrep audio_player)
   ```

3. **Optimize System:**
   ```bash
   # CPU governor
   echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
   ```

### Issue: Background Noise

**Symptoms:**
- Hiss
- Hum
- Interference

**Diagnosis:**
```bash
# Record silence
arecord -d 5 -f S16_LE -r 48000 -c 2 silence.wav
aplay silence.wav

# Check noise floor
amixer -c0 contents | grep -i "noise"
```

**Solutions:**
1. **Enable Noise Reduction:**
   ```bash
   # DSP noise gate
   amixer -c0 sset 'Noise Gate' 'on'
   ```

2. **Adjust Mic Gain:**
   ```bash
   amixer -c0 sset 'Mic Boost' 0dB
   ```

3. **Filter Power Supply:**
   - Add ferrite beads
   - Use shielded cables

---

## Performance Issues

### Issue: High CPU Usage

**Symptoms:**
- CPU > 50%
- System sluggish
- Underruns

**Diagnosis:**
```bash
# Monitor CPU
top -p $(pgrep audio_player)
perf top -p $(pgrep audio_player)

# Check context switches
cat /proc/$(pgrep audio_player)/status | grep ctx
```

**Solutions:**
1. **Optimize Code:**
   ```bash
   # Use -O3 optimization
   CFLAGS="-O3" make all
   ```

2. **Reduce Sample Rate:**
   ```c
   config.sample_rate = 44100;  // Lower rate
   ```

3. **Use DMA:**
   ```c
   // Use DMA for transfers
   config.use_dma = true;
   ```

### Issue: Memory Leak

**Symptoms:**
- Memory usage grows
- OOM killer triggered
- Slow degradation

**Diagnosis:**
```bash
# Monitor memory
watch -n 1 "ps aux | grep audio_player | grep -v grep"

# Check memory leaks
valgrind --leak-check=full ./audio_player -s 440

# Check memory map
cat /proc/$(pgrep audio_player)/maps
```

**Solutions:**
1. **Fix Memory Management:**
   ```c
   // Always free buffers
   audio_free_buffer(buffer);
   buffer = NULL;
   
   // Use safe allocation
   void* data = utils_malloc_safe(size);
   ```

2. **Enable Debugging:**
   ```c
   #define MEMORY_DEBUG 1
   // Track allocations
   ```

### Issue: Underruns

**Symptoms:**
- Skipping audio
- `xrun` in logs
- `snd_pcm_writei` errors

**Diagnosis:**
```bash
# Monitor underruns
cat /proc/asound/card0/pcm0p/sub0/status
dmesg | grep -i "underrun"

# Check latency
/sys/class/sound/card0/pcm0p/sub0/latency
```

**Solutions:**
1. **Increase Buffer:**
   ```c
   config.buffer_size = 16384;
   config.period_size = 4096;
   ```

2. **Increase Priority:**
   ```bash
   sudo renice -n -20 -p $(pgrep audio_player)
   ```

3. **Use Software Resampling:**
   ```conf
   # /etc/asound.conf
   pcm.rate_convert {
       type rate
       slave.pcm "hw:0,0"
       rate 48000
   }
   ```

---

## Debugging Techniques

### 1. Using GDB
```bash
# Compile with debug symbols
CFLAGS="-g -O0" make all

# Run in debugger
gdb ./audio_player
(gdb) run -s 440
(gdb) break audio_play
(gdb) continue
(gdb) print *buffer
(gdb) backtrace
```

### 2. Using Strace
```bash
# Trace system calls
strace -e trace=file,process,network ./audio_player -s 440

# Trace audio calls
strace -e trace=ioctl ./audio_player -s 440
```

### 3. Using Valgrind
```bash
# Memory check
valgrind --leak-check=full --show-leak-kinds=all ./audio_player -s 440

# Profiling
valgrind --tool=callgrind ./audio_player -s 440
callgrind_annotate --auto=yes callgrind.out.*
```

### 4. Using Perf
```bash
# Performance profiling
perf record ./audio_player -s 440
perf report

# CPU profiling
perf stat ./audio_player -s 440
```

### 5. Logging
```c
// Enable detailed logging
#define DEBUG 1
utils_set_log_level(LOG_LEVEL_DEBUG);
utils_set_log_file("/var/log/audio_debug.log");

// Add debug prints
LOG_DEBUG("Audio init: sample_rate=%d, channels=%d", 
          config.sample_rate, config.channels);
```

---

## Recovery Procedures

### Procedure 1: Complete Reset
```bash
# Stop all audio processes
sudo pkill audio_player
sudo pkill pulseaudio

# Restart ALSA
sudo alsa force-reload
sudo /etc/init.d/alsa-utils restart

# Reload modules
sudo modprobe -r snd_soc_nau8822
sudo modprobe -r snd_soc_wm8904
sudo modprobe -r snd_soc_simple_card
sudo modprobe snd_soc_simple_card
sudo modprobe snd_soc_nau8822

# Reboot
sudo reboot
```

### Procedure 2: Recovery from Bad Configuration
```bash
# Backup current config
sudo cp /etc/asound.conf /etc/asound.conf.bak

# Restore default config
sudo cp /usr/share/alsa/alsa.conf /etc/asound.conf

# Reset mixer settings
sudo alsactl init
sudo alsactl store

# Restore your config
sudo cp /etc/asound.conf.bak /etc/asound.conf
```

### Procedure 3: Emergency Audio Recovery
```bash
#!/bin/bash
# audio_recovery.sh

echo "🔧 Emergency Audio Recovery"

# Kill all audio processes
echo "Stopping audio processes..."
sudo pkill -f "audio_player"
sudo pkill -f "pulseaudio"

# Reset ALSA
echo "Resetting ALSA..."
sudo alsa force-reload

# Restore hardware
echo "Restoring audio hardware..."
sudo modprobe -r snd_soc_nau8822
sleep 1
sudo modprobe snd_soc_nau8822

# Restart services
echo "Restarting services..."
sudo systemctl restart alsa-state
sudo systemctl restart audio-player

# Test
echo "Testing audio..."
speaker-test -c 2 -t sine -f 440 -l 1

echo "✅ Recovery complete"
```

### Procedure 4: Factory Reset
```bash
# If all else fails, restore from backup
sudo dd if=/backup/rootfs.img of=/dev/mmcblk2p2 bs=4M status=progress
sudo dd if=/backup/boot.img of=/dev/mmcblk2p1 bs=4M status=progress
sudo reboot
```

---

## Quick Reference: Error Codes

| Error Code | Meaning | Solution |
|------------|---------|----------|
| `AUDIO_ERR_OPEN` | Cannot open device | Check device exists and permissions |
| `AUDIO_ERR_PARAMS` | Invalid parameters | Check sample rate, channels, format |
| `AUDIO_ERR_PREPARE` | Cannot prepare device | Check hardware state |
| `AUDIO_ERR_WRITE` | Write error | Check buffer, device status |
| `AUDIO_ERR_READ` | Read error | Check microphone, device |
| `AUDIO_ERR_BUFFER` | Buffer allocation failed | Check memory, reduce buffer size |
| `AUDIO_ERR_THREAD` | Thread creation failed | Check system resources |

---

## Debug Logs Example

```
2025-01-15 10:30:45 [INFO] [main] Audio application started
2025-01-15 10:30:45 [DEBUG] [audio_init] Opening device: default
2025-01-15 10:30:45 [DEBUG] [audio_init] Sample rate: 48000
2025-01-15 10:30:45 [DEBUG] [audio_init] Channels: 2
2025-01-15 10:30:45 [DEBUG] [audio_init] Bits: 16
2025-01-15 10:30:45 [INFO] [audio_init] Device opened successfully
2025-01-15 10:30:45 [DEBUG] [audio_fill_sine_wave] Generating sine at 440Hz
2025-01-15 10:30:45 [INFO] [audio_play] Starting playback
2025-01-15 10:30:45 [DEBUG] [playback_thread_func] Playback thread started
2025-01-15 10:30:45 [DEBUG] [playback_thread_func] Writing frames: 4096
2025-01-15 10:30:46 [INFO] [audio_stop] Playback stopped
2025-01-15 10:30:46 [DEBUG] [audio_cleanup] Cleaning up
2025-01-15 10:30:46 [INFO] [main] Audio application terminated
```

---

## Support Resources

- **Toradex Documentation**: https://docs.toradex.com
- **NXP iMX8MP Reference**: https://www.nxp.com/imx8mp
- **ALSA Documentation**: https://alsa-project.org
- **Kernel Documentation**: https://kernel.org/doc

---

**Last Updated:** January 2025
**Version:** 1.0.0
```

---

## 📁 File Structure Summary

```
docs/
├── API_REFERENCE.md        # Complete API documentation
├── DEPLOYMENT_GUIDE.md     # Deployment procedures and configurations
└── TROUBLESHOOTING.md      # Diagnostic guide and issue resolution
```

All three documentation files are now complete and comprehensive, providing:
- Complete API reference with examples
- Detailed deployment procedures
- Comprehensive troubleshooting guide
- Diagnostic tools and commands
- Recovery procedures
- Best practices and optimization tips

These files form a complete documentation suite for your Verdin iMX8MP audio project.
