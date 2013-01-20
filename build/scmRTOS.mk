#+---------------------------------------------------------------------------
#
#  Copyright (c) 2010 Anton Gusev aka AHTOXA (HTTP://AHTOXA.NET)
#
#  File:       makefile
#
#  Contents:   makefile to build arm Cortex-M3 software with gcc
#
#----------------------------------------------------------------------------

#############  program name
	TARGET	= ${PROJECT}

# program version
	VER_MAJOR	= 0
	VER_MINOR	= 1

	TOOL	= arm-none-eabi-

# compile options 
	MCU			= cortex-m3
	OPTIMIZE	= -O2
	USE_LTO		= NO


#defines
	DEFS	= -D$(CHIP) 
	DEFS	+= -DVER_MAJOR=$(VER_MAJOR)
	DEFS	+= -DVER_MINOR=$(VER_MINOR)

###########################################################
#  common part for all my cortex-m3 projects
###########################################################

	BASE		= .
	CC			= $(TOOL)gcc
	CXX			= $(TOOL)g++
	LD			= $(TOOL)g++
	AS			= $(CC) -x assembler-with-cpp
	OBJCOPY		= $(TOOL)objcopy
	OBJDUMP		= $(TOOL)objdump
	SIZE		= $(TOOL)size -d
	FLASHER		= openocd
	RM			= rm -f
	CP			= cp
	MD			= mkdir -p


#  dirs
	SRCDIR		= $(BASE)/src
	OBJDIR		= $(BASE)/build/obj
	EXEDIR		= $(BASE)/build/exe
	LSTDIR		= $(BASE)/build/lst
	PRJDIR		= $(BASE)/build/prj
	BAKDIR		= $(BASE)/build/bak
	MODDIR      = $(SCM_RTOS_DIR)/modules
	PLATFORMDIR = $(SCM_RTOS_DIR)/platforms/$(PLATFORM)/
	CHIPDIR     = $(PLATFORMDIR)/processor
	BOARDDIR    = $(PLATFORMDIR)/boards/${BOARD}
	DRIVERDIR   = $(PLATFORMDIR)/drivers


#files
	HEX		= $(EXEDIR)/$(TARGET).hex
	BIN		= $(EXEDIR)/$(TARGET).bin
	ELF		= $(EXEDIR)/$(TARGET).elf
	MAP		= $(LSTDIR)/$(TARGET).map
	LSS		= $(LSTDIR)/$(TARGET).lss
	OK		= $(EXEDIR)/$(TARGET).ok

# scmRTOS dir
	SCMDIR		= ${SCM_RTOS_DIR}/scmRTOS

#driver path
    MODULES_ABS = $(addprefix $(MODDIR)/, $(MODULES))
    DRIVERS_ABS = $(addprefix $(DRIVERDIR)/, $(DRIVERS))

# CMSIS
	CMCIS2		= ${CHIPDIR}/CMSISv2p00_LPC17xx
 
# linker script (chip dependent)
	LD_SCRIPT	= $(CHIPDIR)/$(CHIP).ld

# source directories (all *.c, *.cpp and *.S files included)
	DIRS	:= $(CHIPDIR)/src
	DIRS	+= $(BOARDDIR)/src
	DIRS	+= $(CMCIS2)/src 
	DIRS	+= $(SCMDIR)/Common $(SCMDIR)/CortexM3
	DIRS	+= $(SCMDIR)/Extensions/Profiler
	DIRS	+= $(DRIVERS_ABS)
	DIRS	+= $(MODULES_ABS)
	DIRS	+= $(SRCDIR)

# includes
    INCS    := -I $(SRCDIR)						# application ot test dir
	INCS    += -I $(SCM_RTOS_DIR)/platforms     # all toplevel driver headers
	INCS	+= -I $(PLATFORMDIR)				# all platform header files 
	INCS	+= -I $(DRIVERDIR)					# all driver header files
	INCS    += -I $(CMCIS2)/inc
	INCS	+= -I $(CHIPDIR) -I $(BOARDDIR)
	INCS	+= -I $(PLATFORMDIR) -I $(DRIVERDIR) 
	INCS	+= -I $(SCMDIR)/Common -I $(SCMDIR)/CortexM3  # inclused for scmRtos
	INCS	+= $(patsubst %, -I "%", $(MODULES_ABS))

# individual source files
	SRCS	:= 

#calc obj files list
	OBJS	:= $(SRCS)
	OBJS	+= $(wildcard $(addsuffix /*.cpp, $(DIRS)))
	OBJS	+= $(wildcard $(addsuffix /*.c, $(DIRS)))
	OBJS	+= $(wildcard $(addsuffix /*.S, $(DIRS)))
	OBJS	:= $(notdir $(OBJS))
	OBJS	:= $(OBJS:.cpp=.o)
	OBJS	:= $(OBJS:.c=.o)
	OBJS	:= $(OBJS:.S=.o)
	OBJS	:= $(patsubst %, $(OBJDIR)/%, $(OBJS))

#files to archive
	ARCFILES	= \
		$(SRCDIR) \
		$(PRJDIR) \
		$(SCMDIR) \
		$(BASE)/makefile \
		$(BASE)/.cproject \
		$(BASE)/.project

# flags
	FLAGS	= -mcpu=$(MCU) -mthumb -D__USE_CMSIS
	FLAGS	+= $(INCS)
	FLAGS	+= -MD
	FLAGS	+= $(DEFS)
	FLAGS	+= -Wa,-adhlns=$(addprefix $(LSTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))

	AFLAGS	= $(FLAGS)

	WARN	=  -Wall -Wextra
	WARN	+= -Wshadow -Wcast-qual -Wcast-align
	WARN	+= -Wredundant-decls
	WARN	+= -Wpointer-arith
	WARN    += -Wno-write-strings  # suppress  warning: deprecated conversion from string constant to 'char*
#	WARN	+= -Wunreachable-code
#	WARN	+= -pedantic
	
	CFLAGS	= $(FLAGS)
	CFLAGS	+= $(OPTIMIZE)
	CFLAGS	+= -std=gnu99
	CFLAGS	+= -g
	CFLAGS	+= -ffunction-sections -fdata-sections
	CFLAGS	+= $(WARN)
	CFLAGS	+= -Wimplicit
	CFLAGS	+= -Wnested-externs 

	CXXFLAGS	= $(FLAGS)
	CXXFLAGS	+= $(OPTIMIZE)
	CXXFLAGS	+= -g
	CXXFLAGS	+= -fno-exceptions -fno-rtti
	CXXFLAGS	+= -ffunction-sections -fdata-sections
	CXXFLAGS	+= -fno-threadsafe-statics
	CXXFLAGS	+= -funsigned-bitfields -fshort-enums
	CXXFLAGS	+= $(WARN)
	CXXFLAGS	+= -Winline

	LD_FLAGS	= -mcpu=$(MCU)
	LD_FLAGS	+= -mthumb
	LD_FLAGS	+= -nostartfiles 
	LD_FLAGS	+= -Wl,-Map="$(MAP)",--cref
	LD_FLAGS	+= -Wl,--gc-sections
	LD_FLAGS	+= -T$(LD_SCRIPT)
#	LD_FLAGS	+=  -l stdc++  
#	LD_FLAGS	+= -L /Users/anton/arm-cs-tools/arm-none-eabi/lib/thumb2/   -v


ifeq ($(USE_LTO),YES)
	CFLAGS		+= -flto
	CXXFLAGS	+= -flto
	LD_FLAGS	+= -flto $(OPTIMIZE)
endif

########       openocd command-line

# reset all hardware by SRST 
	oocd_params_srst	= -c "reset_config srst_only"
	oocd_params_srst	+= -c "adapter_nsrst_assert_width 10"
	oocd_params_srst	+= -c "adapter_nsrst_delay 2"

# debug level (d0..d3)
	oocd_params		= -d0
# interface and board/target settings (using the OOCD target-library here)
#	oocd_params		+= -c "fast enable"
	oocd_params		+= -f interface/jlink.cfg
	oocd_params		+= -f $(COMMON)/lpc1766.cfg
	#oocd_params		+= $(oocd_params_srst)
	oocd_params		+= -c init -c targets

	oocd_params_program	= $(oocd_params)
# commands to prepare flash-write
	oocd_params_program	+= -c "halt"
# flash-write and -verify
	oocd_params_program	+= -c "flash write_image erase $(ELF)"
# openOCD adds CSUM in addresses 0x1C..0x1F for write only, not for verify
# TODO: Insert CSUM into $(EXEDIR)/$(TARGET).bin by srecord 
#	oocd_params_program	+= -c "verify_image $(ELF)"
# reset target
	oocd_params_program	+= -c "reset run"
# terminate OOCD after programming
	oocd_params_program	+= -c shutdown

# just reset 
	oocd_params_reset	= $(oocd_params)
	oocd_params_reset	+= -c "reset run"
	oocd_params_reset	+= -c shutdown

.SILENT :

.PHONY: all start dirs build clean program reset archive

############# targets

all : start dirs $(ELF) $(BIN) $(LSS) $(OK)

build: clean all

deploy: $(BIN)
	@echo == copy binary $(BIN) to mount dir: $(MOUNT_DIR)
	cp $(BIN)   $(MOUNT_DIR)

start:
	@echo --- building $(TARGET)
	@echo --- modules: $(MODULES)
#	@echo --- include: $(INCS)
#	@echo --- dirs:    $(DIRS)
#	@echo --- objs:    $(OBJS)

$(LSS): $(ELF) makefile
	@echo --- making asm-lst...
#	@$(OBJDUMP) -dStC $(ELF) > $(LSS)
	@$(OBJDUMP) -dC $(ELF) > $(LSS)

$(OK): $(ELF)
	@$(SIZE) $(ELF)
	@echo "Errors: none"

$(ELF):	$(OBJS) makefile
	@echo --- linking...
	@echo $(LD) $(OBJS) $(LIBS) $(LD_FLAGS) -o "$(ELF)"
	$(LD) $(OBJS) $(LIBS) $(LD_FLAGS) -o "$(ELF)"

$(HEX): $(ELF)
	@echo --- make hex...
	@$(OBJCOPY) -O ihex $(ELF) $(HEX)

$(BIN): $(ELF)
	@echo --- make binary...
	@$(OBJCOPY) -O binary $(ELF) $(BIN)

program: $(ELF)
	@echo "Programming with OPENOCD"
	$(FLASHER) $(oocd_params_program)

reset:
	@echo Resetting device
	$(FLASHER) $(oocd_params_reset)

VPATH := $(DIRS)

$(OBJDIR)/%.o: %.cpp makefile
	@echo --- compiling $<...
#	@echo $(CXX) -c $(CXXFLAGS) -o $@ $<
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.c makefile
	@echo --- compiling $<...
#	@echo $(CC) -c $(CFLAGS) -o $@ $<
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.S makefile
	@echo --- assembling $<...
	$(AS) -c $(AFLAGS) -o $@ $<

dirs: $(OBJDIR) $(EXEDIR) $(LSTDIR) $(BAKDIR)


$(OBJDIR):
	-@$(MD) $(OBJDIR)

$(EXEDIR):
	-@$(MD) $(EXEDIR)

$(LSTDIR):
	-@$(MD) $(LSTDIR)

$(BAKDIR):
	-@$(MD) $(BAKDIR)

clean:
	-@$(RM) -r $(BASE)/build 

archive:
	@echo --- archiving...
	7z a $(BAKDIR)/$(TARGET)_`date +%Y-%m-%d,%H-%M-%S` $(ARCFILES)
	@echo --- done!



# dependencies
ifeq (,$(findstring build,$(MAKECMDGOALS)))
 ifeq (,$(findstring clean,$(MAKECMDGOALS)))
  ifeq (,$(findstring dirs,$(MAKECMDGOALS)))
  -include $(wildcard $(OBJDIR)/*.d) 
  endif
 endif
endif
