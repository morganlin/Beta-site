local_dir 	:= src/NOR-MXIC
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= nor-mxic-menu.c
ifeq "y" "$(CONFIG_PC9220)"
local_src	+= nor-mxic-ctrl.c
endif
ifeq "y" "$(CONFIG_PC9223)"
local_src	+= nor-mxic-ctrl.c 
endif
ifeq "y" "$(CONFIG_PC9002)"
local_src	+= nor-mxic-ctrl.c 
endif
ifeq "y" "$(CONFIG_MDK3D)"
local_src	+= nor-amd-ctrl.c 
endif
ifeq "y" "$(CONFIG_MDKFHD)"
local_src	+= nor-amd-ctrl.c
endif
ifeq "y" "$(CONFIG_INR_PC7230)"
local_src	+= nor-amd-ctrl.c
endif
ifeq "y" "$(CONFIG_PC7210)"
local_src	+= nor-amd-ctrl.c
endif
lib_src		:= 

include build/common.mk
