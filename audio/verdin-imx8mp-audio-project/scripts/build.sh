#!/bin/bash
# Build script for cross-compilation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Configuration
ARCH="aarch64"
TARGET_TRIPLE="aarch64-linux-gnu"
TOOLCHAIN_PATH="/opt/toradex/gcc-11.3.0-aarch64"  # Adjust this path

echo -e "${GREEN}Building audio application for Verdin iMX8MP${NC}"

# Check for cross-compilation
if [ -d "$TOOLCHAIN_PATH" ]; then
    echo "Using cross-compilation toolchain: $TOOLCHAIN_PATH"
    export PATH="$TOOLCHAIN_PATH/bin:$PATH"
    export CC="${TARGET_TRIPLE}-gcc"
    export CXX="${TARGET_TRIPLE}-g++"
    export AR="${TARGET_TRIPLE}-ar"
    export LD="${TARGET_TRIPLE}-ld"
else
    echo "Using native compilation"
    export CC="gcc"
fi

# Clean previous build
echo "Cleaning previous build..."
make clean

# Build
echo "Building application..."
make all

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build successful!${NC}"
    echo "Binary located at: ./bin/audio_player"
    
    # Show binary info
    file ./bin/audio_player
else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi
