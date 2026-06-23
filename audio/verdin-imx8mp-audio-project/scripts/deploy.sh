#!/bin/bash
# Deployment script

set -e

# Configuration
TARGET_IP="192.168.1.100"  # Change to your board's IP
TARGET_USER="torizon"
TARGET_PATH="/home/torizon/apps/audio"
BINARY="bin/audio_player"
CONFIG_FILE="configs/asound.conf"

echo "Deploying to Verdin iMX8MP at $TARGET_IP"

# Create directory on target
ssh ${TARGET_USER}@${TARGET_IP} "mkdir -p ${TARGET_PATH}"

# Copy binary
echo "Copying binary..."
scp ${BINARY} ${TARGET_USER}@${TARGET_IP}:${TARGET_PATH}/

# Copy configuration
if [ -f "$CONFIG_FILE" ]; then
    echo "Copying configuration..."
    scp ${CONFIG_FILE} ${TARGET_USER}@${TARGET_IP}:/etc/asound.conf
fi

# Make executable
ssh ${TARGET_USER}@${TARGET_IP} "chmod +x ${TARGET_PATH}/$(basename ${BINARY})"

echo -e "\nDeployment complete!"
echo "Run with: ssh ${TARGET_USER}@${TARGET_IP} 'cd ${TARGET_PATH} && ./$(basename ${BINARY}) -s 440'"
