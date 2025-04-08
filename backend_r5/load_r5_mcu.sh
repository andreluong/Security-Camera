#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

FIRMWARE_NAME="zephyr_mcu_neopixel.elf"
REMOTE_PATH="/mnt/remote/r5/${FIRMWARE_NAME}"
FIRMWARE_DEST="/lib/firmware/${FIRMWARE_NAME}"
RPROC_FW_PATH="/sys/class/remoteproc/remoteproc2/firmware"
RPROC_STATE_PATH="/sys/class/remoteproc/remoteproc2/state"

echo "------------------------------------------"
echo "Installing Zephyr R5 Firmware for NeoPixel"
echo "------------------------------------------"

echo "Copying firmware to /lib/firmware/..."
sudo cp "$REMOTE_PATH" "$FIRMWARE_DEST"

echo "Loading firmware name into remoteproc..."
echo "$FIRMWARE_NAME" | sudo tee "$RPROC_FW_PATH"

echo "Starting remote processor..."
echo start | sudo tee "$RPROC_STATE_PATH"

echo "Applying MUX and GPIO setup for NeoPixel..."
# These pins are specific to BeagleY-AI and your LED strip setup
gpioset gpiochip0 9=1   # PYMNL.9 (MUX?)
#gpioset gpiochip0 7=1  # Enable LED stick
gpioget gpiochip0 10    # Rotary encoder (trigger gpioget to finish config)

echo "✅ R5 firmware loaded and NeoPixel should now be running."
