local_dir	:= src/OTG/device
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir      := #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= otg-dev-main.c demo_d.c dev_main.c vusbhs_dev_main.c 
		#vusbhs_dev/vusbhs_dev_utl.c \
		#vusbhs_dev/vusbhs_dev_cncl.c \
		#dev_utl.c dev_recv.c dev_cncl.c 
		#dev_send.c	
lib_src		:= 

include build/common.mk

