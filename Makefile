# Variables
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200
PORT := $(shell ls /dev/tty.usbmodem* 2>/dev/null || ls /dev/tty.usbserial* 2>/dev/null)
PROGRAMMER = arduino
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
SRC = $(SRC_DIR)/traffic_light.c
OBJ = $(BUILD_DIR)/traffic_light.elf
HEX = $(BUILD_DIR)/traffic_light.hex

# Compiler and flags
CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Wall -Os -I$(INCLUDE_DIR)
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Default target: Compile, upload and clean
all: compile upload clean

# Compile to ELF and HEX
compile: $(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) $(SRC)
	$(OBJCOPY) -O ihex -R .eeprom $(OBJ) $(HEX)
	@echo "Compilation complete. HEX file created: $(HEX)"

# Upload using avrdude
upload: $(HEX)
	@if [ -z "$(PORT)" ]; then \
		echo "Arduino not found. Make sure it's connected and try again."; \
		exit 1; \
	fi
	$(AVRDUDE) -v -p$(MCU) -c$(PROGRAMMER) -P $(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i
	@echo "Upload complete."

# Clean up generated files
clean:
	rm -f $(OBJ) $(HEX)
	@echo "Cleaned up."

# Phony targets
.PHONY: all compile upload clean
