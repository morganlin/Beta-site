local_dir 	:= platform/arch
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= tlb_mapping.s irq.c pc9223-scu.c
lib_src		:= 

include build/common.mk
