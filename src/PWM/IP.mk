local_dir 	:= src/PWM
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= pwmt-ctrl.c pwmt-menu.c pwmt.c
lib_src		:= 

include build/common.mk
