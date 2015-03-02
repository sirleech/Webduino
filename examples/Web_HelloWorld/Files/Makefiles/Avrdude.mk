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


# AVRDUDE
# ----------------------------------
#
# First /dev port
#
AVRDUDE_PORT      = $(firstword $(wildcard $(BOARD_PORT)))

ifndef AVRDUDE_PATH
    AVRDUDE_PATH      = $(APPLICATION_PATH)/hardware/tools
endif
ifndef AVRDUDE
    AVRDUDE           = $(AVRDUDE_PATH)/avr/bin/avrdude
endif
ifndef AVRDUDE_CONF
    AVRDUDE_CONF      = $(AVRDUDE_PATH)/avr/etc/avrdude.conf
endif
ifndef AVRDUDE_COM_OPTS
    AVRDUDE_COM_OPTS  = -q -V -F -p$(MCU) -C$(AVRDUDE_CONF)
endif
ifndef AVRDUDE_OPTS
    AVRDUDE_OPTS      = -c$(AVRDUDE_PROGRAMMER) -b$(AVRDUDE_BAUDRATE) -P$(AVRDUDE_PORT)
endif

ifndef ISP_PROG
    ISP_PROG	      = -c stk500v2
endif
AVRDUDE_ISP_OPTS  = -P $(ISP_PORT) $(ISP_PROG)

# normal programming info
#
ifndef AVRDUDE_PROGRAMMER
AVRDUDE_PROGRAMMER = $(call PARSE_BOARD,$(BOARD_TAG),upload.protocol)
endif

ifndef AVRDUDE_BAUDRATE
AVRDUDE_BAUDRATE   = $(call PARSE_BOARD,$(BOARD_TAG),upload.speed)
endif

# fuses if you're using e.g. ISP
#
ifndef ISP_LOCK_FUSE_PRE
ISP_LOCK_FUSE_PRE  = $(call PARSE_BOARD,$(BOARD_TAG),bootloader.unlock_bits)
endif

ifndef ISP_LOCK_FUSE_POST
ISP_LOCK_FUSE_POST = $(call PARSE_BOARD,$(BOARD_TAG),bootloader.lock_bits)
endif

ifndef ISP_HIGH_FUSE
ISP_HIGH_FUSE      = $(call PARSE_BOARD,$(BOARD_TAG),bootloader.high_fuses)
endif

ifndef ISP_LOW_FUSE
ISP_LOW_FUSE       = $(call PARSE_BOARD,$(BOARD_TAG),bootloader.low_fuses)
endif

ifndef ISP_EXT_FUSE
ISP_EXT_FUSE       = $(call PARSE_BOARD,$(BOARD_TAG),bootloader.extended_fuses)
endif

ifndef VARIANT
VARIANT            = $(call PARSE_BOARD,$(BOARD_TAG),build.variant)
endif
