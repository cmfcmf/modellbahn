# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

BOARD_TAG    = nano
BOARD_SUB 	 = atmega328old
MONITOR_PORT = /dev/ttyUSB0

include ../../Arduino-Makefile/Arduino.mk

test:
	python tests/main.py
	gtkwave /tmp/simulavr_trace.vcd