#!/bin/bash

ADDRESS=$(grep -E "[A-F0-9]+\s+[A-F0-9]+\s+ABORT_LABEL_RED_IS_HERE\\$:" build/main.rst | grep -oE "[0-9A-F]+" | head -n 1)
echo "Found address $ADDRESS"
sed -i "/REPLACE_ADDRESS_RED_MARKER/c\      \"$ADDRESS\" // REPLACE_ADDRESS_RED_MARKER" measure-pwm.h

ADDRESS=$(grep -E "[A-F0-9]+\s+[A-F0-9]+\s+ABORT_LABEL_GREEN_IS_HERE\\$:" build/main.rst | grep -oE "[0-9A-F]+" | head -n 1)
echo "Found address $ADDRESS"
sed -i "/REPLACE_ADDRESS_GREEN_MARKER/c\      \"$ADDRESS\" // REPLACE_ADDRESS_GREEN_MARKER" measure-pwm.h
