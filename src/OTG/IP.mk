local_dir	:= src/OTG
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= $(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= otg-ctrl.c otg-menu.c  
lib_src		:= 

include build/common.mk

