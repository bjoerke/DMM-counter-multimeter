#
# Build options
#
TARGET = main.hex
DEVICE = atmega1284p
LIBS = 
CC = avr-gcc
PORT = /dev/ttyUSB0
BUILD_DIR = build
CFLAGS = -Wall -mmcu=$(DEVICE) -DF_CPU=16000000UL -Os -std=gnu99
LFLAGS = -mmcu=$(DEVICE)
#
#
#

_OBJECTS = $(patsubst %.c, %.o, $(wildcard **/*.c)) $(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS =  $(foreach object, $(_OBJECTS), $(BUILD_DIR)/$(object))
HEADERS = $(wildcard **/*.c)                       $(wildcard *.h)

.PHONY: default all clean upload

default: upload

all: default

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -Wall $(LIBS) -o $@
	avr-objcopy -O ihex -R .eeprom $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/$(TARGET)

upload: $(BUILD_DIR)/$(TARGET)
	avrdude -V -F -u -v -c stk500v1 -p $(DEVICE) -P $(PORT) -U flash:w:main.hex

clean:
	rm -rf $(BUILD_DIR)
	-rm -f $(TARGET)
