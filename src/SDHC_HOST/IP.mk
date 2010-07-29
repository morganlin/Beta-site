local_dir	:= src/SDHC_HOST
lib_dir		:= $(local_dir)
lib_name	:= #??.a
header_dir	:= #$(local_dir)/include

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
#local_src	:= sdhc-ctrl.c socle-sdmmc.c sdhc-menu.c socle-sdhc.c 
local_src	:= sdhc-ctrl.c sdhc-menu.c socle-sdhc.c
lib_src		:= 

include build/common.mk
