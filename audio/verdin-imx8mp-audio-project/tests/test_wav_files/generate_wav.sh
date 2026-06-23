#!/bin/bash
# Generate test WAV files using sox (if available)

echo "🎵 Generating test WAV files using sox..."

# Check if sox is installed
if ! command -v sox &> /dev/null; then
    echo "⚠️  sox not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y sox
fi

# Function to generate with fallback
generate_wav() {
    local filename=$1
    local synth_params=$2
    
    if sox -n "$filename" $synth_params 2>/dev/null; then
        echo "✅ Generated: $filename"
        return 0
    else
        echo "❌ Failed: $filename"
        return 1
    fi
}

# Generate various test files
echo ""
generate_wav "test_440hz.wav" "synth 3 sine 440"
generate_wav "test_1000hz.wav" "synth 2 sine 1000"
generate_wav "test_50hz.wav" "synth 3 sine 50"
generate_wav "test_silence.wav" "synth 3 sine 0"
generate_wav "test_square_440.wav" "synth 2 square 440"
generate_wav "test_sawtooth_440.wav" "synth 2 sawtooth 440"
generate_wav "test_triangle_440.wav" "synth 2 triangle 440"
generate_wav "test_sweep.wav" "synth 3 sine 20-20000"

# Stereo
sox -n "test_stereo.wav" synth 3 sine 440 synth 3 sine 880 channels 2 2>/dev/null
echo "✅ Generated: test_stereo.wav"

# Generate multi-tone (using sox effects)
sox -n "test_complex.wav" \
    synth 3 sine 440 \
    synth 0.5 square 554 \
    synth 1.0 triangle 659 \
    synth 0.5 sine 880 \
    norm -1 2>/dev/null
echo "✅ Generated: test_complex.wav"

# Generate white noise
sox -n "test_noise.wav" synth 3 whitenoise 2>/dev/null
echo "✅ Generated: test_noise.wav"

# Generate pink noise
sox -n "test_pinknoise.wav" synth 3 pinknoise 2>/dev/null
echo "✅ Generated: test_pinknoise.wav"

# Generate impulse
sox -n "test_impulse.wav" synth 0.1 sine 1 2>/dev/null
sox "test_impulse.wav" "test_impulse2.wav" fade 0 0.1 0.01 2>/dev/null
mv "test_impulse2.wav" "test_impulse.wav" 2>/dev/null
echo "✅ Generated: test_impulse.wav"

echo ""
echo "✅ All WAV files generated using sox!"
ls -lh *.wav
