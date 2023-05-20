NAME = HomeController
CHIP = esp32
BOARD = nodemcu-32s
BUILD_DIR = build-$(BOARD)
SKETCH = $(NAME).ino
USE_CCACHE = 1
VERBOSE = 0
include ~/makeEspArduino/makeEspArduino.mk

all:
	cp $(BUILD_DIR)/$(NAME).bin /sdcard/ArduinoDroid/precompiled
