local_dir 	:= src/MAC
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= macctrl.c mac-menu.c if_socle.c EEP25x.c
lib_src		:= 

include build/common.mk
