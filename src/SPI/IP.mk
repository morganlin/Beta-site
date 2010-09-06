local_dir 	:= src/SPI
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= spi-master.c eeprom-test.c marvel-wifi-test.c tsc2000-test.c sq-spi-slave-test.c spi-master-menu.c spi-master-ctrl.c spi-slave-ctrl.c spi-slave-menu.c ads7846-test.c max1110-test.c max1110-menu.c

ifeq "y" "$(CONFIG_PANEL48)"
local_src += sam48-lcd-init.c
endif

lib_src		:= 

include build/common.mk
