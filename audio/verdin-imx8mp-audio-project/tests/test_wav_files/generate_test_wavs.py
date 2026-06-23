#!/usr/bin/env python3
"""
Generate test WAV files for Verdin iMX8MP audio testing
"""

import struct
import math
import wave
import numpy as np

def generate_sine_wave(filename, frequency, duration, sample_rate=48000, amplitude=0.5):
    """Generate a sine wave WAV file"""
    num_samples = int(sample_rate * duration)
    
    # Generate sine wave
    samples = []
    for i in range(num_samples):
        value = amplitude * math.sin(2.0 * math.pi * frequency * i / sample_rate)
        # Convert to 16-bit PCM
        pcm_value = int(value * 32767)
        samples.append(pcm_value)
    
    # Write WAV file
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 16-bit
        wav_file.setframerate(sample_rate)
        
        # Convert samples to bytes
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated: {filename}")
    print(f"   Frequency: {frequency}Hz, Duration: {duration}s, Sample Rate: {sample_rate}Hz")

def generate_stereo_sine_wave(filename, freq_left, freq_right, duration, sample_rate=48000):
    """Generate stereo sine wave with different frequencies on each channel"""
    num_samples = int(sample_rate * duration)
    
    # Generate samples
    samples_left = []
    samples_right = []
    for i in range(num_samples):
        left = 0.5 * math.sin(2.0 * math.pi * freq_left * i / sample_rate)
        right = 0.5 * math.sin(2.0 * math.pi * freq_right * i / sample_rate)
        samples_left.append(int(left * 32767))
        samples_right.append(int(right * 32767))
    
    # Write WAV file
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(2)  # Stereo
        wav_file.setsampwidth(2)  # 16-bit
        wav_file.setframerate(sample_rate)
        
        # Interleave samples
        audio_data = b''
        for left, right in zip(samples_left, samples_right):
            audio_data += struct.pack('<hh', left, right)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated stereo: {filename}")
    print(f"   Left: {freq_left}Hz, Right: {freq_right}Hz, Duration: {duration}s")

def generate_silence(filename, duration, sample_rate=48000):
    """Generate silence WAV file"""
    num_samples = int(sample_rate * duration)
    audio_data = b'\x00' * (num_samples * 2)  # 16-bit silence
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated silence: {filename}")
    print(f"   Duration: {duration}s, Sample Rate: {sample_rate}Hz")

def generate_sweep(filename, start_freq, end_freq, duration, sample_rate=48000):
    """Generate frequency sweep WAV file"""
    num_samples = int(sample_rate * duration)
    
    samples = []
    for i in range(num_samples):
        t = i / sample_rate
        freq = start_freq + (end_freq - start_freq) * (t / duration)
        value = 0.5 * math.sin(2.0 * math.pi * freq * t)
        samples.append(int(value * 32767))
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated sweep: {filename}")
    print(f"   {start_freq}Hz -> {end_freq}Hz, Duration: {duration}s")

def generate_impulse(filename, sample_rate=48000):
    """Generate impulse/click WAV file"""
    duration = 0.1  # 100ms
    num_samples = int(sample_rate * duration)
    
    samples = [0] * num_samples
    samples[0] = 32767  # Full scale impulse at start
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated impulse: {filename}")

def generate_square_wave(filename, frequency, duration, sample_rate=48000, amplitude=0.5):
    """Generate square wave WAV file"""
    num_samples = int(sample_rate * duration)
    
    samples = []
    period_samples = int(sample_rate / frequency)
    half_period = period_samples // 2
    
    for i in range(num_samples):
        if (i % period_samples) < half_period:
            value = amplitude
        else:
            value = -amplitude
        samples.append(int(value * 32767))
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated square wave: {filename}")
    print(f"   Frequency: {frequency}Hz, Duration: {duration}s")

def generate_complex_test(filename, sample_rate=48000):
    """Generate complex test signal with multiple tones"""
    duration = 3.0
    num_samples = int(sample_rate * duration)
    
    # Mix multiple frequencies
    tones = [
        (440, 0.3),   # A4
        (554, 0.2),   # C#5
        (659, 0.2),   # E5
        (880, 0.15),  # A5
    ]
    
    samples = []
    for i in range(num_samples):
        t = i / sample_rate
        value = 0
        for freq, amp in tones:
            value += amp * math.sin(2.0 * math.pi * freq * t)
        # Normalize
        value = max(-1.0, min(1.0, value))
        samples.append(int(value * 32767))
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated complex test: {filename}")
    print(f"   Duration: {duration}s, Tones: {', '.join([str(f) for f, _ in tones])}Hz")

def generate_voice_sample(filename, sample_rate=48000):
    """Generate a sample that simulates voice frequencies"""
    duration = 2.0
    num_samples = int(sample_rate * duration)
    
    # Simulate vowel-like sounds with formants
    # Formant frequencies for 'a' vowel
    formants = [(730, 0.8), (1090, 0.7), (2440, 0.4)]
    
    samples = []
    for i in range(num_samples):
        t = i / sample_rate
        value = 0
        for freq, amp in formants:
            # Add some vibrato
            vibrato = 0.02 * math.sin(2.0 * math.pi * 5 * t)
            freq_vibrato = freq * (1 + vibrato)
            value += amp * math.sin(2.0 * math.pi * freq_vibrato * t)
        # Add pitch (A3)
        pitch = 0.3 * math.sin(2.0 * math.pi * 220 * t)
        value += pitch
        
        # Apply envelope
        envelope = min(1.0, 2.0 * t / 0.1) if t < 0.1 else 1.0
        if t > duration - 0.2:
            envelope = 1.0 - (t - (duration - 0.2)) / 0.2
        value *= envelope
        
        # Normalize
        value = max(-1.0, min(1.0, value))
        samples.append(int(value * 32767))
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        audio_data = b''
        for sample in samples:
            audio_data += struct.pack('<h', sample)
        
        wav_file.writeframes(audio_data)
    
    print(f"✅ Generated voice sample: {filename}")
    print(f"   Duration: {duration}s, Simulating 'a' vowel")

def main():
    """Generate all test WAV files"""
    print("🎵 Generating test WAV files for Verdin iMX8MP\n")
    
    # Basic tests
    generate_sine_wave("test_440hz.wav", 440, 3.0)
    print()
    generate_silence("test_silence.wav", 3.0)
    print()
    
    # Additional useful test files
    generate_sine_wave("test_1000hz.wav", 1000, 2.0)
    print()
    generate_sine_wave("test_50hz.wav", 50, 3.0)  # Bass test
    print()
    generate_square_wave("test_square_440.wav", 440, 2.0)
    print()
    generate_sweep("test_sweep.wav", 20, 20000, 3.0)
    print()
    generate_impulse("test_impulse.wav")
    print()
    generate_stereo_sine_wave("test_stereo.wav", 440, 880, 3.0)
    print()
    generate_complex_test("test_complex.wav")
    print()
    generate_voice_sample("test_voice.wav")
    
    print("\n✅ All test WAV files generated successfully!")
    print("📁 Files:")
    print("   - test_440hz.wav    : Pure sine wave at 440Hz")
    print("   - test_1000hz.wav   : Pure sine wave at 1kHz")
    print("   - test_50hz.wav     : Bass test at 50Hz")
    print("   - test_silence.wav  : 3 seconds of silence")
    print("   - test_square_440.wav : Square wave at 440Hz")
    print("   - test_sweep.wav    : Frequency sweep 20Hz-20kHz")
    print("   - test_impulse.wav  : Impulse/click test")
    print("   - test_stereo.wav   : Stereo test (440Hz left, 880Hz right)")
    print("   - test_complex.wav  : Complex multi-tone test")
    print("   - test_voice.wav    : Simulated voice sample")

if __name__ == "__main__":
    main()
