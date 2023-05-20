BOARD_TAG = uno
ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR = /usr/share/arduino
USER_LIB_PATH = /root/Arduino/libraries
ARDUINO_LIBS = IRremote
include ${ARDMK_DIR}/Arduino.mk

all:
	cp build-${BOARD_TAG}/HomeController.hex /sdcard/ArduinoDroid/precompiled
