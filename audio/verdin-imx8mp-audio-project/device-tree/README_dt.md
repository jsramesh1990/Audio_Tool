# Device Tree Overlay for Verdin iMX8MP Audio

## Overview
This Device Tree overlay enables audio support for the Verdin iMX8MP module with various audio codecs including NAU8822 and WM8904.

## Supported Codecs
- **NAU8822**: Default codec on Verdin Development Board
- **WM8904**: Alternative codec on some carrier boards
- **Generic I2S**: Can be adapted for other I2S codecs

## Prerequisites
- Verdin iMX8MP module
- Linux kernel with Device Tree overlay support
- I2C and SAI enabled in base Device Tree

## Compilation

### On the Target Board
```bash
# Install Device Tree compiler if not present
sudo apt-get install device-tree-compiler

# Compile the overlay
dtc -@ -I dts -O dtb -o imx8mp-verdin-audio-overlay.dtbo imx8mp-verdin-audio-overlay.dts
