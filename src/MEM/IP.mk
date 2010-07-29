local_dir 	:= src/MEM
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= mem-ctrl.c mem-menu.c #cmd_bootm.c zlib.c
lib_src		:= 

include build/common.mk
