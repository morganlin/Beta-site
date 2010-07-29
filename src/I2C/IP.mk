local_dir	:= src/I2C
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= i2c.c i2c-ctrl.c i2c-menu.c \
				TVP5145.c \
				CH700X.c\
				cat6613.c cat6023.c \
				mt9v125.c \

ifeq "y" "$(CONFIG_LCD)"
local_src +=  CH7026.c 
endif

lib_src		:= 

include build/common.mk
