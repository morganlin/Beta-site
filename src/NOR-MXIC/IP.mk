local_dir 	:= src/NOR-MXIC
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= nor-mxic-menu.c
ifeq "y" "$(CONFIG_PC9223)"
local_src	+= nor-mxic-ctrl.c 
endif
lib_src		:= 

include build/common.mk
