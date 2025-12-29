# Audio Processing Tool - C++ Pitch Shifter & EQ Filter

![C++](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.10%2B-green?style=for-the-badge&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20WSL-lightgrey?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)

[![Libsndfile](https://img.shields.io/badge/libsndfile-✓-success?style=flat-square)](http://www.mega-nerd.com/libsndfile/)
[![Offline Processing](https://img.shields.io/badge/Offline%20Processing-✓-blue?style=flat-square)](https://en.wikipedia.org/wiki/Offline_processing)
[![WAV Support](https://img.shields.io/badge/WAV%20Support-✓-important?style=flat-square)](https://en.wikipedia.org/wiki/WAV)

A professional-grade C++ audio processing tool for offline audio manipulation. Features pitch shifting via resampling and multiple biquad filter types for precise equalization. Perfect for audio editing, music production, and audio effects processing.

##  Table of Contents
- [Overview](#-overview)
- [Features](#-features)
- [System Requirements](#-system-requirements)
- [Installation](#-installation)
- [Build Instructions](#-build-instructions)
- [Usage](#-usage)
- [Audio Effects](#-audio-effects)
- [Examples](#-examples)
- [Project Structure](#-project-structure)
- [Technical Details](#-technical-details)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)

##  Overview

This tool provides a command-line interface for processing WAV audio files with various effects:
- **Pitch Shifting**: Change musical pitch by semitones (preserves tempo)
- **Biquad Filters**: Professional-grade audio filters including:
  - Low Shelf (bass boost/cut)
  - High Pass (remove low frequencies)
  - Peaking EQ (bell curve for precise frequency adjustment)

All processing is done offline for highest quality and portability across systems.

##  Features

###  **Audio Effects**
- **Pitch Shifting**: -24 to +24 semitones range
- **Low Shelf Filter**: Adjust bass frequencies (20Hz - 500Hz)
- **High Pass Filter**: Remove rumble and low-end noise
- **Peaking EQ**: Boost or cut specific frequency bands

###  **Audio Quality**
- 32-bit floating point internal processing
- Professional biquad filter implementation
- High-quality resampling for pitch shifting
- Preserves original sample rate metadata

###  **File Support**
- WAV format (16-bit, 24-bit, 32-bit float)
- Mono and stereo support
- Sample rates: 8kHz to 192kHz
- Automatic format detection

###  **Performance**
- Offline processing for maximum quality
- Multi-threaded processing (future enhancement)
- Efficient memory usage
- Progress indicators

##  System Requirements

### Minimum Requirements
![Minimum Specs](https://img.shields.io/badge/Minimum%20Specs-✓-lightgrey?style=flat-square)

- **OS**: Linux, macOS, or Windows (WSL2)
- **CPU**: x86-64 or ARM64 processor
- **RAM**: 512 MB minimum
- **Storage**: 50 MB free space

### Development Requirements
![Dev Requirements](https://img.shields.io/badge/Dev%20Requirements-C++17%2B-blueviolet?style=flat-square)

```bash
# Debian/Ubuntu
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    libsndfile1-dev \
    pkg-config

# macOS (Homebrew)
brew install \
    cmake \
    libsndfile \
    pkg-config

# Fedora/RHEL
sudo dnf install \
    gcc-c++ \
    cmake \
    libsndfile-devel \
    pkg-config
```

##  Installation

### Quick Install (Linux/macOS)
```bash
# Clone the repository
git clone https://github.com/yourusername/audio-tool.git
cd audio-tool

# One-line build and install
./install.sh
```

### Manual Installation
```bash
# 1. Create build directory
mkdir -p build && cd build

# 2. Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# 3. Build the project
make -j$(nproc)

# 4. Install (optional)
sudo make install
```

### Verify Installation
```bash
# Check if executable is available
audio_tool --version
# or
./build/audio_tool --help
```

##  Build Instructions

### Standard Build
```bash
mkdir build && cd build
cmake ..
make
```

### Debug Build
```bash
mkdir debug && cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Release Build (Optimized)
```bash
mkdir release && cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Installation Paths
```bash
# Custom installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# Build with specific compiler
cmake -DCMAKE_CXX_COMPILER=g++-11 ..
```

##  Usage

### Basic Syntax
```bash
audio_tool -i INPUT.wav -o OUTPUT.wav [OPTIONS]
```

### Command Line Options
| Option | Arguments | Description | Default |
|--------|-----------|-------------|---------|
| `-i`, `--input` | `FILE.wav` | Input audio file | Required |
| `-o`, `--output` | `FILE.wav` | Output audio file | Required |
| `--pitch` | `semitones` | Pitch shift amount (-24 to +24) | 0 |
| `--lowshelf` | `freq gain Q` | Low shelf filter (Hz, dB, Q) | None |
| `--peak` | `freq gain Q` | Peaking EQ filter (Hz, dB, Q) | None |
| `--hp` | `freq Q` | High-pass filter (Hz, Q) | None |
| `--gain` | `dB` | Overall gain adjustment | 0 |
| `--normalize` | - | Normalize output to -1 dBFS | Off |
| `--progress` | - | Show processing progress | On |
| `--version` | - | Display version info | - |
| `--help` | - | Show help message | - |

##  Audio Effects

### Pitch Shifting (`--pitch`)
![Pitch Shifting](https://img.shields.io/badge/Pitch%20Shifting-✓-9cf?style=flat-square)

```bash
# Examples:
--pitch 3       # Raise pitch by 3 semitones
--pitch -7      # Lower pitch by 7 semitones
--pitch 12      # Raise by one octave
```

**Parameters:**
- Range: -24 to +24 semitones
- Algorithm: Resampling with anti-aliasing
- Quality: High-quality sinc interpolation

### Low Shelf Filter (`--lowshelf`)
![Bass Control](https://img.shields.io/badge/Bass%20Control-✓-orange?style=flat-square)

```bash
# Format: --lowshelf <frequency> <gain_dB> <Q>
--lowshelf 100 6 0.7    # Boost bass at 100Hz by +6dB
--lowshelf 80 -4 1.0    # Cut bass at 80Hz by -4dB
```

**Parameters:**
- Frequency: 20Hz - 500Hz (corner frequency)
- Gain: -24dB to +24dB
- Q: 0.1 to 10 (bandwidth/resonance)

### High Pass Filter (`--hp`)
![High Pass](https://img.shields.io/badge/High%20Pass-✓-blue?style=flat-square)

```bash
# Format: --hp <frequency> <Q>
--hp 80 0.707     # Remove frequencies below 80Hz
--hp 120 1.0      # More aggressive high-pass at 120Hz
```

**Parameters:**
- Frequency: 20Hz - 1000Hz (cutoff frequency)
- Q: 0.1 to 10 (filter steepness)

### Peaking EQ (`--peak`)
![Peaking EQ](https://img.shields.io/badge/Peaking%20EQ-✓-violet?style=flat-square)

```bash
# Format: --peak <frequency> <gain_dB> <Q>
--peak 1000 -3 2.0    # Cut 1kHz by -3dB
--peak 3000 4 1.5     # Boost 3kHz by +4dB
```

**Parameters:**
- Frequency: 20Hz - 20000Hz (center frequency)
- Gain: -24dB to +24dB
- Q: 0.1 to 10 (bandwidth)

##  Examples

### 1. Basic Pitch Shift
```bash
# Raise pitch by 3 semitones (minor third)
./audio_tool -i vocals.wav -o pitched.wav --pitch 3

# Lower pitch by 7 semitones (perfect fifth)
./audio_tool -i guitar.wav -o lower.wav --pitch -7
```

### 2. Bass Enhancement
```bash
# Boost bass at 100Hz +6dB with smooth Q
./audio_tool -i track.wav -o boosted.wav --lowshelf 100 6 0.7

# Add bass and treble adjustment
./audio_tool -i track.wav -o eq.wav --lowshelf 80 4 0.8 --peak 5000 2 1.2
```

### 3. Professional Mastering Chain
```bash
# Complete processing example:
./audio_tool -i raw_recording.wav -o mastered.wav \
    --pitch 0 \
    --hp 40 0.707 \
    --lowshelf 120 2 0.8 \
    --peak 800 -1 1.5 \
    --peak 3000 1 2.0 \
    --peak 10000 2 1.0 \
    --normalize
```

### 4. Vocal Processing
```bash
# Vocal processing chain
./audio_tool -i vocal_dry.wav -o vocal_processed.wav \
    --pitch 2 \
    --hp 80 0.7 \
    --peak 200 -2 1.8 \
    --peak 2500 3 1.5 \
    --peak 8000 2 2.0
```

### 5. Drum Processing
```bash
# Kick drum enhancement
./audio_tool -i kick.wav -o kick_processed.wav \
    --hp 30 0.707 \
    --peak 60 6 2.0 \
    --peak 200 -3 1.0

# Snare drum processing
./audio_tool -i snare.wav -o snare_processed.wav \
    --peak 200 2 1.5 \
    --peak 5000 3 2.0
```

##  Project Structure

```
audio-tool/
├── CMakeLists.txt              # Build configuration
├── src/
│   ├── main.cpp               # Command-line interface
│   ├── AudioProcessor.cpp     # Main processing class
│   ├── AudioProcessor.h
│   ├── BiquadFilter.cpp       # Filter implementation
│   ├── BiquadFilter.h
│   ├── PitchShifter.cpp       # Pitch shifting algorithm
│   ├── PitchShifter.h
│   └── WavFile.cpp           # WAV I/O wrapper
│   └── WavFile.h
├── include/                   # Public headers
├── examples/                  # Usage examples
│   ├── basic_usage.sh
│   ├── mastering_chain.sh
│   └── vocal_processing.sh
├── tests/                    # Unit tests
├── docs/                    # Documentation
├── scripts/                 # Utility scripts
└── README.md               # This file
```

##  Technical Details

### Biquad Filter Implementation
```cpp
// Direct Form II implementation
class BiquadFilter {
private:
    double b0, b1, b2, a1, a2;
    double x1, x2, y1, y2;
    
public:
    void setLowShelf(double freq, double gainDB, double Q, double sampleRate);
    void setHighPass(double freq, double Q, double sampleRate);
    void setPeaking(double freq, double gainDB, double Q, double sampleRate);
    
    double process(double sample);
};
```

### Pitch Shifting Algorithm
- **Method**: Resampling with sinc interpolation
- **Anti-aliasing**: 64-tap FIR filter
- **Quality**: 16x oversampling for smooth results
- **Performance**: O(n) complexity

### Audio Processing Pipeline
1. **Input Stage**: Read WAV file, convert to 32-bit float
2. **Processing Stage**: Apply effects in sequence
3. **Output Stage**: Convert back to original format, write file

##  Troubleshooting

### Common Issues

**1. Libsndfile Not Found**
```bash
# Ubuntu/Debian
sudo apt-get install libsndfile1-dev

# macOS
brew install libsndfile

# Verify installation
pkg-config --libs sndfile
```

**2. CMake Version Too Old**
```bash
# Upgrade CMake
sudo apt-get remove cmake
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:kitware/kitware-ppa
sudo apt-get update
sudo apt-get install cmake
```

**3. Build Errors**
```bash
# Clean and rebuild
rm -rf build
mkdir build && cd build
cmake ..
make VERBOSE=1  # Show detailed build output
```

**4. Runtime Errors**
```bash
# Check file permissions
chmod +x audio_tool

# Check library paths
ldd audio_tool

# Run with debug output
./audio_tool --help
```

### Debug Mode
```bash
# Build with debug symbols
mkdir debug && cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with valgrind for memory checking
valgrind --leak-check=full ./audio_tool -i input.wav -o output.wav
```

##  License

![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

##  Contributing

![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen?style=for-the-badge)

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

##  Resources

### Learning Resources
- [libsndfile Documentation](http://www.mega-nerd.com/libsndfile/)
- [Biquad Filter Cookbook](https://www.w3.org/TR/audio-eq-cookbook/)
- [Audio DSP Fundamentals](https://www.dspguide.com/)

### Related Projects
- [SoX - Sound eXchange](http://sox.sourceforge.net/)
- [FFmpeg](https://ffmpeg.org/)
- [JUCE Audio Framework](https://juce.com/)

##  Features Roadmap

### Planned Features
![Future Features](https://img.shields.io/badge/Future%20Features-✓-informational?style=flat-square)

- [ ] **Multi-band Compression**
- [ ] **Reverb & Delay Effects**
- [ ] **Graphic EQ (10-band)**
- [ ] **Batch Processing**
- [ ] **GUI Interface**
- [ ] **VST Plugin Support**
- [ ] **Real-time Processing**
- [ ] **MP3/FLAC Support**
- [ ] **Spectrum Analyzer**
- [ ] **Noise Reduction**

---

**Enjoy professional audio processing from the command line!** 🎧

[![Star on GitHub](https://img.shields.io/github/stars/yourusername/audio-tool?style=social)](https://github.com/yourusername/audio-tool)
[![Follow](https://img.shields.io/twitter/follow/yourusername?style=social)](https://twitter.com/yourusername)

*For questions and support, please open an issue on GitHub.*
