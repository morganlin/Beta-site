local_dir	:= src/LCD
lib_dir		:=
lib_name	:=
header_dir	:= #$(local_dir)/include

local_lib	:=

# compile source
local_src	:= color_fill.c screen.c pwm-ctrl.c

ifeq "y" "$(CONFIG_PC9220)"
local_src += socle/socle-lcd-ctrl.c socle/socle-lcd-menu.c
endif

ifeq "y" "$(CONFIG_PC9223)"
local_src += socle/socle-lcd-ctrl.c socle/socle-lcd-menu.c
endif

ifeq "y" "$(CONFIG_PC7210)"
local_src += socle/socle-lcd-ctrl.c socle/socle-lcd-menu.c
endif

ifeq "y" "$(CONFIG_PC9002)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_MDK3D)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_MDKFHD)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_CDK)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_PDK)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_MSMV)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

ifeq "y" "$(CONFIG_SCDK)"
local_src += arm/arm-lcd-ctrl.c arm/arm-lcd-menu.c
endif

include build/common.mk
