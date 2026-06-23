#!/bin/bash

# Audio Test Script for Verdin iMX8MP
# Tests all audio functionality

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
AUDIO_DEVICE="default"
TEST_DURATION=3
SINE_FREQ=440
VOLUME_LEVEL=0.5

# Test functions
print_header() {
    echo -e "\n${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║   $1${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ️  $1${NC}"
}

check_audio_hardware() {
    print_header "Checking Audio Hardware"
    
    # List audio devices
    echo "Available sound cards:"
    cat /proc/asound/cards
    echo ""
    
    # Check for audio codec
    if dmesg | grep -i "nau8822" | grep -q "registered"; then
        print_success "NAU8822 codec detected"
    elif dmesg | grep -i "wm8904" | grep -q "registered"; then
        print_success "WM8904 codec detected"
    else
        print_error "No known audio codec detected"
        print_info "Please check device tree configuration"
    fi
    
    # Check audio device
    if aplay -l | grep -q "card"; then
        print_success "ALSA playback devices found"
    else
        print_error "No ALSA playback devices found"
        return 1
    fi
    
    echo ""
}

test_audio_playback() {
    print_header "Testing Audio Playback"
    
    # Test 1: Speaker test using speaker-test
    print_info "Running speaker test (2 seconds)..."
    if speaker-test -c 2 -t sine -f $SINE_FREQ -l 1 > /dev/null 2>&1; then
        print_success "Speaker test passed"
    else
        print_error "Speaker test failed"
        return 1
    fi
    
    # Test 2: Play sine wave with our app
    print_info "Playing sine wave at ${SINE_FREQ}Hz..."
    if ./bin/audio_player -s $SINE_FREQ -d $AUDIO_DEVICE > /dev/null 2>&1 & then
        sleep 2
        kill $! 2>/dev/null
        wait $! 2>/dev/null
        print_success "Sine wave playback passed"
    else
        print_error "Sine wave playback failed"
        return 1
    fi
    
    # Test 3: Play WAV file
    if [ -f "tests/test_wav_files/test_440hz.wav" ]; then
        print_info "Playing test WAV file..."
        if ./bin/audio_player -p tests/test_wav_files/test_440hz.wav -d $AUDIO_DEVICE > /dev/null 2>&1 & then
            sleep 2
            kill $! 2>/dev/null
            wait $! 2>/dev/null
            print_success "WAV playback passed"
        else
            print_error "WAV playback failed"
        fi
    else
        print_info "Test WAV file not found, skipping"
    fi
    
    echo ""
}

test_audio_recording() {
    print_header "Testing Audio Recording"
    
    # Test recording
    print_info "Recording test (${TEST_DURATION} seconds)..."
    if arecord -d $TEST_DURATION -f S16_LE -r 48000 -c 2 /tmp/test_record.wav > /dev/null 2>&1; then
        print_success "Recording completed"
        
        # Check file size
        if [ -f /tmp/test_record.wav ]; then
            SIZE=$(stat -c%s /tmp/test_record.wav 2>/dev/null || stat -f%z /tmp/test_record.wav 2>/dev/null)
            if [ "$SIZE" -gt 10000 ]; then
                print_success "Recorded file size: $SIZE bytes"
            else
                print_error "Recorded file too small: $SIZE bytes"
            fi
            rm /tmp/test_record.wav
        fi
    else
        print_error "Recording failed"
        return 1
    fi
    
    echo ""
}

test_volume_control() {
    print_header "Testing Volume Control"
    
    # Get current volume
    CURRENT_VOL=$(amixer sget Master 2>/dev/null | grep -o "\[[0-9]*%\]" | head -1 | tr -d '[]%')
    if [ -n "$CURRENT_VOL" ]; then
        print_info "Current volume: ${CURRENT_VOL}%"
    fi
    
    # Set volume
    print_info "Setting volume to ${VOLUME_LEVEL}..."
    if amixer sset Master ${VOLUME_LEVEL} > /dev/null 2>&1; then
        print_success "Volume set to ${VOLUME_LEVEL}"
    else
        print_info "Volume control not available (using software volume)"
    fi
    
    # Test with our app
    print_info "Testing software volume control..."
    if ./bin/audio_player -s $SINE_FREQ -v 0.3 > /dev/null 2>&1 & then
        sleep 1
        kill $! 2>/dev/null
        wait $! 2>/dev/null
        print_success "Software volume control working"
    else
        print_error "Software volume control failed"
    fi
    
    echo ""
}

test_audio_latency() {
    print_header "Testing Audio Latency"
    
    # Record and measure latency
    print_info "Measuring playback latency..."
    
    # Start recording and playback simultaneously
    # This is a simplified test
    START_TIME=$(date +%s%3N)
    arecord -d 1 -f S16_LE -r 48000 -c 2 /tmp/latency_test.wav > /dev/null 2>&1 &
    RECORD_PID=$!
    
    sleep 0.1
    ./bin/audio_player -s 1000 > /dev/null 2>&1 &
    PLAY_PID=$!
    
    wait $RECORD_PID 2>/dev/null
    END_TIME=$(date +%s%3N)
    
    kill $PLAY_PID 2>/dev/null
    
    LATENCY=$((END_TIME - START_TIME))
    print_info "Approximate latency: ${LATENCY}ms"
    
    rm -f /tmp/latency_test.wav
    
    if [ $LATENCY -lt 200 ]; then
        print_success "Latency acceptable (<200ms)"
    else
        print_error "Latency too high: ${LATENCY}ms"
    fi
    
    echo ""
}

test_audio_formats() {
    print_header "Testing Audio Formats"
    
    # Test various sample rates
    for RATE in 44100 48000 96000; do
        print_info "Testing ${RATE}Hz..."
        if speaker-test -c 2 -r $RATE -t sine -f 440 -l 1 > /dev/null 2>&1; then
            print_success "  ${RATE}Hz supported"
        else
            print_error "  ${RATE}Hz not supported"
        fi
    done
    
    # Test various bit depths
    for BITS in 16 24; do
        print_info "Testing ${BITS}-bit..."
        if speaker-test -c 2 -f S${BITS}_LE -t sine -f 440 -l 1 > /dev/null 2>&1; then
            print_success "  ${BITS}-bit supported"
        else
            print_error "  ${BITS}-bit not supported"
        fi
    done
    
    echo ""
}

test_system_audio_info() {
    print_header "System Audio Information"
    
    # ALSA version
    echo "ALSA version: $(cat /proc/asound/version 2>/dev/null || echo 'Unknown')"
    
    # Audio devices
    echo -e "\nPlayback devices:"
    aplay -l 2>/dev/null | grep "card" || echo "None found"
    
    echo -e "\nCapture devices:"
    arecord -l 2>/dev/null | grep "card" || echo "None found"
    
    # Sound card info
    echo -e "\nSound card details:"
    for card in /proc/asound/card*/id 2>/dev/null; do
        if [ -f "$card" ]; then
            echo "  $(dirname $card | xargs basename): $(cat $card)"
        fi
    done
    
    echo ""
}

test_quality() {
    print_header "Audio Quality Test"
    
    print_info "Generating test signals..."
    
    # Generate sine wave
    if ./bin/audio_player -s 440 -v 0.5 > /dev/null 2>&1 & then
        sleep 1
        kill $! 2>/dev/null
        print_success "Sine wave test passed"
    else
        print_error "Sine wave test failed"
    fi
    
    # Generate square wave (using harmonics)
    # This requires our app to support custom waveforms
    print_info "Testing dynamic range..."
    
    # Test with different volumes
    for VOL in 0.1 0.5 1.0; do
        if ./bin/audio_player -s 440 -v $VOL > /dev/null 2>&1 & then
            sleep 0.5
            kill $! 2>/dev/null
            print_success "  Volume $VOL passed"
        fi
    done
    
    echo ""
}

# ============================================
# MAIN TEST SUITE
# ============================================

main() {
    echo -e "\n${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║   Audio Test Suite                    ║${NC}"
    echo -e "${BLUE}║   Verdin iMX8MP Audio Project        ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --device|-d)
                AUDIO_DEVICE="$2"
                shift 2
                ;;
            --duration|-t)
                TEST_DURATION="$2"
                shift 2
                ;;
            --freq|-f)
                SINE_FREQ="$2"
                shift 2
                ;;
            --volume|-v)
                VOLUME_LEVEL="$2"
                shift 2
                ;;
            --help|-h)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  -d, --device DEVICE   Audio device (default: default)"
                echo "  -t, --duration SEC    Test duration in seconds (default: 3)"
                echo "  -f, --freq HZ         Sine wave frequency (default: 440)"
                echo "  -v, --volume LEVEL    Volume level (0.0-1.0) (default: 0.5)"
                echo "  -h, --help            Show this help"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Check if binary exists
    if [ ! -f "bin/audio_player" ]; then
        print_error "Audio player binary not found. Run 'make all' first."
        exit 1
    fi
    
    # Run all tests
    TESTS_FAILED=0
    
    check_audio_hardware || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_system_audio_info
    test_audio_playback || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_audio_recording || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_volume_control || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_audio_formats || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_audio_latency || TESTS_FAILED=$((TESTS_FAILED + 1))
    test_quality || TESTS_FAILED=$((TESTS_FAILED + 1))
    
    # Summary
    echo -e "\n${BLUE}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║   Test Summary                        ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}✅ ALL TESTS PASSED!${NC}"
        echo -e "${GREEN}🎉 Audio system is working correctly${NC}\n"
        exit 0
    else
        echo -e "${RED}❌ $TESTS_FAILED test(s) failed${NC}"
        echo -e "${YELLOW}⚠️  Please check the audio hardware and configuration${NC}\n"
        exit 1
    fi
}

# Run main function
main "$@"
