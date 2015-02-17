LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ARCH := or1k

MODULE_DEPS += \
	lib/cbuf \
	dev/interrupt/or1k_pic \
	dev/timer/or1k_ticktimer

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)/include

MODULE_SRCS += \
	$(LOCAL_DIR)/platform.c \
	$(LOCAL_DIR)/uart.c

MEMBASE ?= 0x00000000
MEMSIZE ?= 0x02000000

GLOBAL_DEFINES += \
	MEMBASE=$(MEMBASE) \
	MEMSIZE=$(MEMSIZE)

include make/module.mk
