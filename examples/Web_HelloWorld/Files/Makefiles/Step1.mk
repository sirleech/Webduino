#
# embedXcode
# ----------------------------------
# Embedded Computing on Xcode 4
#
# Copyright © Rei VILO, 2010-2012
# Licence CC = BY NC SA
#

# References and contribution
# ----------------------------------
# See About folder
# 


# Sketch unicity test and extension
# ----------------------------------
#

ifneq ($(SKETCH_EXTENSION),cpp)

ifeq ($(words $(wildcard *.$(SKETCH_EXTENSION))), 0)
    $(error No $(SKETCH_EXTENSION) sketch)
endif

ifneq ($(words $(wildcard *.$(SKETCH_EXTENSION))), 1)
    $(error More than one $(SKETCH_EXTENSION) sketch)
endif

endif



# Early info
#
ifneq ($(TARGET_NAME),clean)
$(info  ---- info ----)
$(info Project)
$(info .    target		$(TARGET_NAME))
$(info .    name		$(PROJECT_NAME))
$(info .    tag 		$(BOARD_TAG))
$(info .    extension	$(SKETCH_EXTENSION))
$(info .    library path $(USER_LIB_PATH))
endif


# Board selection
# ----------------------------------
# Board specifics defined in .xconfig file
# BOARD_TAG and AVRDUDE_PORT 
#
ifneq ($(TARGET_NAME),clean)
ifndef BOARD_TAG
    $(error BOARD_TAG not defined)
endif
endif

ifndef BOARD_PORT
    BOARD_PORT = /dev/tty.usb*
endif


# Arduino.app Mpide.app Wiring.app Energia.app paths
#
ARDUINO_APP = /Applications/Arduino.app
MPIDE_APP   = /Applications/Mpide.app
WIRING_APP  = /Applications/Wiring.app
ENERGIA_APP = /Applications/Energia.app
MAPLE_APP   = /Applications/MapleIDE.app

ifeq ($(wildcard $(ARDUINO_APP)),)
ifeq ($(wildcard $(MPIDE_APP)),)
ifeq ($(wildcard $(WIRING_APP)),)
ifeq ($(wildcard $(ENERGIA_APP)),)
ifeq ($(wildcard $(MAPLE_APP)),)
    $(error Error: no application found)
endif
endif
endif
endif
endif

ARDUINO_PATH = $(ARDUINO_APP)/Contents/Resources/Java
MPIDE_PATH   = $(MPIDE_APP)/Contents/Resources/Java
WIRING_PATH  = $(WIRING_APP)/Contents/Resources/Java
ENERGIA_PATH = $(ENERGIA_APP)/Contents/Resources/Java
MAPLE_PATH   = $(MAPLE_APP)/Contents/Resources/Java


# Miscellaneous
# ----------------------------------
# Variables
#
TARGET       = $(PROJECT)

USER_PATH   := $(wildcard ~)
$(info .    user		$(USER_PATH))

# main.cpp selection
# = 1 takes local main.cpp
#
#NO_CORE_MAIN_FUNCTION := 1

# Builds directory
#
OBJDIR  = Builds

# Function PARSE_BOARD data retrieval from boards.txt
# result = $(call READ_BOARD_TXT,'boardname','parameter')
#
#$(info *** grep ^$(1).$(2) $(BOARDS_TXT) | cut -d = -f 2 )
PARSE_BOARD = $(shell grep ^$(1).$(2) $(BOARDS_TXT) | cut -d = -f 2 )


# Clean if new BOARD_TAG
# ----------------------------------
#
NEW_TAG := $(strip $(OBJDIR)/$(BOARD_TAG)-TAG) #
OLD_TAG := $(strip $(wildcard $(OBJDIR)/*-TAG)) # */

ifneq ($(OLD_TAG),$(NEW_TAG))
    CHANGE_FLAG := 0 // hack, TODO: corbin, fix flipping this..
else
    CHANGE_FLAG := 0
endif


# Identification and switch
# ----------------------------------
# Look if BOARD_TAG is listed as a Arduino/Arduino board
# Look if BOARD_TAG is listed as a Mpide/PIC32 board
# Look if BOARD_TAG is listed as a Wiring/Wiring board
# Look if BOARD_TAG is listed as a Energia/MPS430 board
# Look if BOARD_TAG is listed as a MapleIDE/LeafLabs board
# Order matters!
#
ifneq ($(TARGET_NAME),boards)
ifneq ($(TARGET_NAME),clean)
ifneq ($(shell grep ^$(BOARD_TAG).name $(ARDUINO_PATH)/hardware/teensy/boards.txt),)
    include $(MAKEFILE_PATH)/Teensy.mk
else ifneq ($(shell grep ^$(BOARD_TAG).name $(ARDUINO_PATH)/hardware/arduino/boards.txt),)
    include $(MAKEFILE_PATH)/Arduino.mk
else ifneq ($(shell grep ^$(BOARD_TAG).name $(MPIDE_PATH)/hardware/pic32/boards.txt),)
    include $(MAKEFILE_PATH)/Mpide.mk
else ifneq ($(shell grep ^$(BOARD_TAG).name $(WIRING_PATH)/hardware/Wiring/boards.txt),)
    include $(MAKEFILE_PATH)/Wiring.mk
else ifneq ($(shell grep ^$(BOARD_TAG).name $(ENERGIA_PATH)/hardware/msp430/boards.txt),)
    include $(MAKEFILE_PATH)/Energia.mk
else ifneq ($(shell grep ^$(BOARD_TAG).name $(MAPLE_PATH)/hardware/leaflabs/boards.txt),)
    include $(MAKEFILE_PATH)/MapleIDE.mk
else
    $(error $(BOARD_TAG) is unknown)
endif
endif
endif

ifneq ($(TARGET_NAME),boards)
ifneq ($(TARGET_NAME),clean)
ifneq ($(PLATFORM),Wiring)
$(info .    ide	     	$(PLATFORM))
ifneq ($(PLATFORM),MapleIDE)
$(info .    version 	$(shell cat $(APPLICATION_PATH)/lib/version.txt))
else
$(info .    version 	$(shell cat $(APPLICATION_PATH)/lib/build-version.txt))
endif
endif
endif
endif


# List of sub-paths to be excluded
#
EXCLUDE_NAMES  = Example example Examples examples Archive archive Archives archives Documentation documentation Reference reference
EXCLUDE_NAMES += ArduinoTestSuite OneWire USBCore USBDesc
EXCLUDE_LIST   = $(addprefix %,$(EXCLUDE_NAMES))

# Step 2
#
include $(MAKEFILE_PATH)/Step2.mk
