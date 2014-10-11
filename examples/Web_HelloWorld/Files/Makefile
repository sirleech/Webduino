#
# embedXcode
# ----------------------------------
# Embedded Computing on Xcode 4
#
# Copyright © Rei VILO, 2010-2012
# Licence CC = BY NC SA
#


# Parameters
# ----------------------------------
# Xcode takes BOARD_TAG and BOARD_PORT from the .xcconfig file
# For other IDEs than Xcode,

# BOARD_PORT is optional
# If not defined, BOARD_PORT = /dev/tty.usb* (default)
#

# Xcode takes SKETCHBOOK_DIR from preferences.txt 
# If not defined, each IDE has its own SKETCHBOOK_DIR (default)
# To share the same SKETCHBOOK_DIR along all IDEs, define
# 
#SKETCHBOOK_DIR =

# SERIAL_BAUDRATE for the serial console, 9600 by default
# Uncomment and specify another speed
#
#SERIAL_BAUDRATE = 19200


# Libraries
# ----------------------------------
# Declare application Arduino/chipKIT/Wiring/Energia/Maple and users libraries used 
# Short-listing libraries speeds-up building
# Typing = 0 takes none
# Otherwise, leaving empty considers all (default)

# List Arduino libraries here
#
#APP_LIBS_LIST = Wire Wire/utility EEPROM Ethernet Ethernet/utility	SPI Firmata LiquidCrystal Matrix Sprite SD SD/utility Servo SoftwareSerial Stepper
#APP_LIBS_LIST = Wire Wire/utility EEPROM  SoftwareSerial
#APP_LIBS_LIST = Wire Wire/utility

APP_LIBS_LIST = SPI Wire Wire/utility

# List users libraries here -- edit this list for what i'm using in my user libs...
USER_LIBS_LIST = Webduino Adafruit_CC3000_Library Adafruit_CC3000_Library/utility

MAKEFILE_PATH  = $(CURDIR)/Makefiles
UTILITIES_PATH = $(CURDIR)/Utilities

include $(MAKEFILE_PATH)/Step1.mk
