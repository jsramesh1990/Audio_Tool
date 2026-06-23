# Test WAV Files

This directory contains test audio files for the Verdin iMX8MP audio project.

## Available Files

| File | Description | Duration | Frequency |
|------|-------------|----------|-----------|
| `test_440hz.wav` | Pure sine wave at A4 | 3s | 440Hz |
| `test_1000hz.wav` | Pure sine wave at 1kHz | 2s | 1000Hz |
| `test_50hz.wav` | Bass test tone | 3s | 50Hz |
| `test_silence.wav` | Silence | 3s | - |
| `test_square_440.wav` | Square wave | 2s | 440Hz |
| `test_sweep.wav` | Frequency sweep | 3s | 20Hz-20kHz |
| `test_impulse.wav` | Impulse/click | 0.1s | - |
| `test_stereo.wav` | Stereo test | 3s | 440Hz/880Hz |
| `test_complex.wav` | Multi-tone | 3s | Various |
| `test_voice.wav` | Simulated voice | 2s | - |
| `test_noise.wav` | White noise | 3s | - |
| `test_pinknoise.wav` | Pink noise | 3s | - |

## Regenerating Test Files

### Option 1: Python Script
```bash
python3 generate_test_wavs.py

Option 2: Shell Script (requires sox)
bash

chmod +x generate_wav.sh
./generate_wav.sh

Option 3: C Program
bash

gcc -o generate_wav generate_wav.c -lm
./generate_wav

Test Usage Examples
Test Playback
bash

# Play sine wave
aplay test_440hz.wav

# Play stereo test
aplay test_stereo.wav

# Use our audio player
./bin/audio_player -p test_440hz.wav

# Record and compare
arecord -d 3 -f S16_LE -r 48000 -c 2 recorded.wav
aplay recorded.wav

Test Audio Quality
bash

# Test frequency response
aplay test_sweep.wav

# Test stereo separation
aplay test_stereo.wav

# Test noise floor
aplay test_silence.wav

File Specifications

    Format: PCM WAV

    Sample Rate: 48kHz

    Bit Depth: 16-bit

    Channels: Mono (unless specified)

    Endianness: Little-endian

Generated Files Size

    3 seconds mono @ 48kHz/16-bit = ~288KB

    3 seconds stereo @ 48kHz/16-bit = ~576KB

text


---

## **Quick Generation Command (One-liner)**
```bash
# Generate all test files with sox (if installed)
cd tests/test_wav_files
for freq in 440 1000 50; do sox -n "test_${freq}hz.wav" synth 3 sine $freq; done
sox -n test_silence.wav synth 3 sine 0
sox -n test_stereo.wav synth 3 sine 440 synth 3 sine 880 channels 2
echo "✅ All test files generated!"

Now your test WAV files are ready to use! You can:

    Use Python: python3 generate_test_wavs.py

    Use Bash: ./generate_wav.sh (requires sox)

    Use C: gcc -o generate_wav generate_wav.c -lm && ./generate_wav

    One-liner: The quick command above

All generated files will be proper WAV files that can be used for testing your Verdin iMX8MP audio system.
