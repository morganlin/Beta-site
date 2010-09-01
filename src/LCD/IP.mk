local_dir	:= src/LCD
lib_dir		:=
lib_name	:=
header_dir	:= #$(local_dir)/include

local_lib	:=

# compile source
local_src	:= color_fill.c screen.c pwm-ctrl.c

ifeq "y" "$(CONFIG_PC9223)"
local_src += socle/socle-lcd-ctrl.c socle/socle-lcd-menu.c
endif

include build/common.mk
