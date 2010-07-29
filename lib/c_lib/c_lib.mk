local_dir 	:= lib/c_lib
lib_dir		:= lib
lib_name	:= libc.a

local_lib	:= $(lib_dir)/$(lib_name)

# compile source
local_src	:= 
lib_src		:= atob.c buffer.c genlib.c isdigit.c qsort.c sprintf.c str_fmt.c strcspn.c strichr.c \
			div0.c _ashldi3.s _ashrdi3.s _divsi3.s _modsi3.s _umodsi3.s _udivsi3.s

include build/lib_common.mk
