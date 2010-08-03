# Makefile for Socle Diagnostic Program
#
# Copyright (C) 2003-2009 Socle Technology Corp.
#

Q		= @
REVISION	= $(shell cat .revision)

obj_dir		:= build/obj
objs		:= src/main.o src/main-menu.o
srcs		:= src/main.c src/main-menu.c
libs		:= 
depend		= $(subst .c,.d,$(srcs))
cp_obj_dir	:= src CPU/arch #lib #lib is for old version make
clean_dir	:= src CPU platform lib
objs_grp	= $(shell find $(obj_dir) -name '*.o')
include_dir	:= ./src ./include ./include/lib ./include/platform/arch
EXTRA_CFLAGS	:= 
text_base	:= 0x00000000
cfg_name_lists	:= sq32c8006b 
targetcfg	:= $(findstring $(MAKECMDGOALS),$(cfg_name_lists))
diagch		:= diag_config.h
toppath		:=$(shell pwd)
destpath	:=$(toppath)/include

SED		= sed
MV		= mv -f
RM		= rm -f
ARFLAGS		= qsv

all:

# Tool Chain
# ---------------------------------------------------------------------------
# ARM7
#CROSS		= arm-7-linux-uclibc-

# ARM9
#CROSS		= armv5te-softfloat-linux-

# ARM11
#CROSS		= arm-socle-linux-gnueabi-
#CROSS		= arm-11-linux-uclibcgnueabi-
CROSS		= arm-none-linux-gnueabi-

NM		= $(CROSS)nm
AR		= $(CROSS)ar
LD		= $(CROSS)ld
ASM		= $(CROSS)as
CC		= $(CROSS)gcc
CXX		= $(CROSS)g++
OBJCOPY		= $(CROSS)objcopy
OBJDUMP		= $(CROSS)objdump
ASMCPP		= $(CROSS)cpp
RANLIB		= $(CROSS)ranlib

LDFLAGS		= -L$(dir $(shell $(CC) -print-libgcc-file-name)) -lgcc
# ---------------------------------------------------------------------------

ifeq ($(MAKECMDGOALS),$(findstring $(MAKECMDGOALS),$(cfg_name_lists)))
sinclude .config
else
include .config
endif
sinclude .config.cmd
include build/platform.in
include build/CPU.in
include build/IP.in
include CPU/arch/CPU.mk

ifndef CONFIG_ROM
sinclude lib/lib.mk
include platform/arch/platform.mk
cp_obj_dir	+= platform/arch
endif


# CFlags parameter
# ---------------------------------------------------------------------------
# Emdian
ifeq "y" "$(CONFIG_BIG_ENDIAN)"
SYS_ENDIAN	= SYS_CPU_BIG
endif
ifeq "y" "$(CONFIG_LITTLE_ENDIAN)"
SYS_ENDIAN	= SYS_CPU_LITTLE
endif

# SemiHost
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
SEMI_HOST	= SEMI_HOST_FUNCTION
endif
ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
SEMI_HOST	= SEMI_HOST_FUNCTION_ARM
endif

INCLUDE		:= $(addprefix -I,$(include_dir)) -include ./include/diag_config.h
# ---------------------------------------------------------------------------


# Flags
# ---------------------------------------------------------------------------

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
				> /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

ASFLAGS		= -x assembler-with-cpp -D_ASSEMBLER_

# Endian
ifeq "$(SYS_ENDIAN)" "SYS_CPU_BIG"
CFLAGS		+= -mlong-calls -Wall -gstabs3 -O0 -G0 -EB -msoft-float -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE)
LDFLAGS		+= -e start -N -n -EB -Map $@.map
endif


ifeq "$(SYS_ENDIAN)" "SYS_CPU_LITTLE"

# CPU
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
CFLAGS		+= -mlong-calls -Wall -gstabs3 -O0 -G0 -EL -mips32 -mcpu=4kc -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE)
LDFLAGS		+= -e start -N -n -EL -Map $@.map
endif

ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
CFLAGS		+= -mapcs-frame -mpoke-function-name -mlong-calls -Wall -O0  -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE) #-Wstrict-prototypes

ifeq "y" "$(CONFIG_VFP)"
EXTRA_CFLAGS 	+= -s $(call cc-option,-mapcs-32,-mabi=apcs-gnu) $(call cc-option,-mfpu=vfp,) $(call cc-option,-mfloat-abi=softfp,)
else
EXTRA_CFLAGS 	+= $(call cc-option,-mapcs-32,-mabi=apcs-gnu) $(call cc-option,-mfpu=fpa,) $(call cc-option,-mfloat-abi=softfp,)
endif

LDFLAGS		+= -N -n -EL

ifeq "y" "$(CONFIG_ARM11)"
EXTRA_CFLAGS    += $(call cc-option,-march=armv6k,-march=armv6j)
else
CFLAGS		+= -march=armv5te
endif

ifeq "y" "$(CONFIG_MDK3D)"
text_base	:= 0x40000000
endif

ifeq "y" "$(CONFIG_ARM7_HI)"
text_base	:= 0xF0000000
endif

ifeq "y" "$(CONFIG_ARM9_HI)"
text_base	:= 0x4F000000
endif

CFLAGS		+= -DTEXT_BASE=$(text_base)
LDFLAGS		+= -Ttext $(text_base)

endif # endif "$(CPU_ARCH)" "CPU_ARCH_ARM"

# Debuger
ifeq "y" "$(CONFIG_SEMIHOST)"
CFLAGS		+= -gdwarf-2
endif
ifeq "y" "$(CONFIG_GDB)"
CFLAGS		+= -gstabs
endif

endif #endif "$(SYS_ENDIAN)" "SYS_CPU_LITTLE"


# UART display
ifeq "y" "$(CONFIG_UART_DISP)"
CFLAGS		+= -DUART_DEBUG
endif
# ---------------------------------------------------------------------------

# That's our default target when none is given on the command line
all: build-ln $(platform_arch)_$(cpu)_v$(REVISION).out 

.PHONY: help clean release distclean

ld_file		= build/ELF.lds


# Help
# ---------------------------------------------------------------------------
help:
	@echo "  clean			- delete temporary and dependence files created by build"
	@echo ""
	@echo "  distclean		- delete .obj files created by build"
	@echo ""
	@echo "Configuration:"
	@echo "  sq8000		        - interactive curses-based configurator"
	@echo ""
# ---------------------------------------------------------------------------

# Configuration
# ---------------------------------------------------------------------------
#scripts/config/mconf: scripts/config/Makefile
#	@$(MAKE) -s -C scripts/config ncurses conf mconf
#	-@if [ ! -f .config ] ; then \
#		touch .config; \
#	fi

#menuconfig: scripts/config/mconf
#	@$< build/config.in
#	@$(MAKE) -s build-ln

$(targetcfg):
	@if [ -e ".config" ] ;then\
		$(MV) .config .config.old;\
		cp $@.config .config;\
		scripts/config/genh $(toppath) $(destpath);\
	else\
		cp $@.config .config;\
		scripts/config/genh $(toppath) $(destpath);\
	fi;
	@if [ $? ] ;then\
		exit 1;\
		echo "Initialize Configuration fail";\
	fi;
	@echo "Initialize Configuration success"

# ---------------------------------------------------------------------------

# Clean
# ---------------------------------------------------------------------------
clean:
	@echo -n "Cleaning temporary and dependence files... "
	@find $(clean_dir) -name '*.o' | xargs $(RM)
	@find ./ -name '*.d' | xargs $(RM)
	@find ./ -name '*.*~' | xargs $(RM)
	@find ./ -name '*.tmp' | xargs $(RM)
	@$(RM) *.out *.text *.srec *.bin lib/libio.a
	@$(MAKE) -s -C scripts/config $@
	@echo "finish!"
# ---------------------------------------------------------------------------

# Distclean
# ---------------------------------------------------------------------------
distclean: clean
	@echo -n "Cleaning *.o in $(obj_dir)/... "
	@$(RM) $(obj_dir)/*.o $(platform_arch)_$(cpu)*
	@echo "finish!"
# ---------------------------------------------------------------------------

# target
# ---------------------------------------------------------------------------
$(platform_arch)_$(cpu)_v$(REVISION).out: $(objs) $(libs) cp_objs $(objs_grp)
	@echo LD $(ld_file)
	$(Q)$(LD) -T$(ld_file) $(objs_grp) --start-group  $(libs) --end-group $(LDFLAGS) -o $@
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
	$(Q)$(OBJDUMP) -h -S -l -m mips --show-raw-insn -EL $@ > $*.text
endif
ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
	$(Q)$(OBJDUMP) -h -S -l -m arm --show-raw-insn -EL $@ > $*.text
endif
	$(Q)$(OBJCOPY) --gap-fill=0xff -O srec $@ $*.srec
	$(Q)$(OBJCOPY) --gap-fill=0xff -O binary $@ $*.bin
	@echo Output files: $@ $*.text $*.srec $*.bin
# ---------------------------------------------------------------------------

#$(warning cyli print: CPU = $(cpu), platform_arch = $(platform_arch), objs = $(objs))

cp_objs:
	@for d in $(cp_obj_dir);	\
	do						\
		find $$d/ -name '*.o' | xargs cp --target-directory=$(obj_dir)/;	\
	done

build-ln:
	@$(RM) include/platform/arch CPU/arch platform/arch
	@ln -fsn $(platform_arch) include/platform/arch
	@ln -fsn $(cpu) CPU/arch
	@ln -fsn $(platform_arch) platform/arch

ifneq ($(MAKECMDGOALS),$(findstring $(MAKECMDGOALS),$(cfg_name_lists)))
ifneq "$(MAKECMDGOALS)" "help"
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "distclean"
sinclude $(depend)
endif
endif
endif
endif

%.o: %.c
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -MD -c $< -o $@

%.o: %.cpp
	@echo CXX $@
	$(Q)$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) -MD -c $< -o $@

%.o: %.s
	@echo AS $@
	$(Q)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(ASFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# copy to tftp server
pub:
	cp $(platform_arch)_$(cpu)_v$(REVISION).bin /tftpboot/cyli

# DO NOT DELETE
