local_dir	:= src/NAND_NFC
lib_dir		:=
lib_name	:=
header_dir	:= #$(local_dir)/include

local_lib	:=

# compile source
local_src	:= nfc-ctrl.c nfc-ctrl-menu.c socle_nand.c 
lib_src		:=

include build/common.mk
