# This is a minimal set of ANSI/VT100 color codes
_END=\033[0m
_BOLD=\033[1m
_UNDER=\033[4m
_REV=\033[7m

# Colors
_GREY=\033[30m
_RED=\033[31m
_GREEN=\033[32m
_YELLOW=\033[33m
_BLUE=\033[34m
_PURPLE=\033[35m
_CYAN=\033[36m
_WHITE=\033[37m

#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif

UNAME_S	:=	$(shell uname -s)

ifneq (,$(findstring Darwin,$(UNAME_S)))
	export SHELL=/bin/bash
endif

export PATH			:=	$(DEVKITPPC)/bin:$(PORTLIBS)/bin:$(PATH)
export LIBOGC_INC	:=	$(DEVKITPRO)/libogc/include
export LIBOGC_LIB	:=	$(DEVKITPRO)/libogc/lib/wii
export PORTLIBS     := $(DEVKITPRO)/portlibs/wiiu

PREFIX	:=	powerpc-eabi-

export AS	:=	$(PREFIX)as
export CC	:=	$(PREFIX)gcc
export CXX	:=	$(PREFIX)g++
export AR	:=	$(PREFIX)ar
export OBJCOPY	:=	$(PREFIX)objcopy

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	no_ssl_patch
BUILD		:=	build_files
BUILD_DBG	:=	$(TARGET)_dbg
SOURCES		:=	source \
				source/wiiu \
				source/utils \

				
DATA		:=	

INCLUDES	:=  source

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	:=  -nostdlib -std=gnu11 -mrvl -mcpu=750 -meabi -mhard-float -ffast-math \
		    -O2 -Wall -Wextra -D_GNU_SOURCE -Wno-unused-parameter -Wno-strict-aliasing -Wno-array-bounds -Wno-stringop-overflow $(INCLUDE)
CXXFLAGS := -nostdlib -std=gnu++11 -mrvl -mcpu=750 -meabi -mhard-float -ffast-math \
		    -O2 -Wall -Wextra -D_GNU_SOURCE -Wno-unused-parameter -Wno-strict-aliasing -Wno-array-bounds -Wno-stringop-overflow $(INCLUDE)
ASFLAGS	:= -mregnames
LDFLAGS	:= -nostartfiles -Wl,-Map,$(notdir $@).map,--gc-sections,--wrap=malloc,--wrap=free,--wrap=calloc,--wrap=realloc,--wrap=aligned_alloc,--wrap=malloc_usable_size,--wrap=memalign,--wrap=posix_memalign,--wrap=pvalloc,--wrap=valloc

#---------------------------------------------------------------------------------
Q := @
MAKEFLAGS += --no-print-directory
#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(CURDIR)	\
			$(DEVKITPPC)/lib  \
			$(DEVKITPPC)/lib/gcc/powerpc-eabi/8.3.0 \
			$(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export PROJECTDIR := $(CURDIR)
export OUTPUT	:=	$(CURDIR)/$(TARGETDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CCFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cc)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
TTFFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.ttf)))
PNGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.png)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

ifeq ($(strip $(CCFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(CCFILES:.cc=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o) \
					$(PNGFILES:.png=.png.o) $(addsuffix .o,$(BINFILES))

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) -I$(LIBOGC_INC) \
					-I$(PORTLIBS)/include -I$(PORTLIBS)/include/freetype2 \
					-I$(PORTLIBS)/include/dynamic_libs \
					-I$(PORTLIBS)/include/libutils

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB) -L$(PORTLIBS)/lib

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: clean

debug: CFLAGS += -D__DEBUG__
debug: CXXFLAGS += -D__DEBUG__
debug: $(CURDIR)/source/iosu_payload/arm_kernel.h executable_d
release: $(CURDIR)/source/iosu_payload/arm_kernel.h executable_r

#---------------------------------------------------------------------------------

$(CURDIR)/source/iosu_payload/arm_kernel.h: $(CURDIR)/source/iosu_payload/arm_user.h
	@$(MAKE) --no-print-directory -C $(CURDIR)/arm_iosu_code/arm_kernel/ -f  $(CURDIR)/arm_iosu_code/arm_kernel/Makefile

$(CURDIR)/source/iosu_payload/arm_user.h:
	@$(MAKE) --no-print-directory -C $(CURDIR)/arm_iosu_code/arm_user/ -f  $(CURDIR)/arm_iosu_code/arm_user/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo -e "${_CYAN}Cleaning the project${_END}"
	@$(MAKE) --no-print-directory -C $(CURDIR)/arm_iosu_code/arm_kernel -f  $(CURDIR)/arm_iosu_code/arm_kernel/Makefile clean
	@$(MAKE) --no-print-directory -C $(CURDIR)/arm_iosu_code/arm_user -f  $(CURDIR)/arm_iosu_code/arm_user/Makefile clean
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).bin $(BUILD_DBG).elf $(CURDIR)/source/iosu_payload/arm_kernel.h $(CURDIR)/source/iosu_payload/arm_user.h
#---------------------------------------------------------------------------------
executable_d:
	@echo -e "\n${_YELLOW}Building for dev/debug${_END}"
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@$(MAKE) debug --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
#---------------------------------------------------------------------------------
executable_r:
	@echo -e "\n${_YELLOW}Building for release${_END}"
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@$(MAKE) release --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
#---------------------------------------------------------------------------------

else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).elf:  $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------

debug: CFLAGS += -D__DEBUG__
debug: CXXFLAGS += -D__DEBUG__
debug: $(OUTPUT).elf
release: $(OUTPUT).elf

%.elf: link.ld $(OFILES)
	@echo -e "${_PURPLE}[LD]${_END}  Linking ${_CYAN}$(notdir $@)${_END}.."
	$(Q)$(LD) -n -T $^ $(LDFLAGS) -o ../$(BUILD_DBG).elf  $(LIBPATHS) $(LIBS)
	$(Q)$(OBJCOPY) -S -R .comment -R .gnu.attributes ../$(BUILD_DBG).elf $@

#---------------------------------------------------------------------------------
%.a:
#---------------------------------------------------------------------------------
	@echo $(notdir $@)
	@rm -f $@
	@$(AR) -rc $@ $^

#---------------------------------------------------------------------------------
%.o: %.cpp
	@echo -e "${_GREEN}[C++]${_END} Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.cc
	@echo -e "${_GREEN}[C++]${_END} Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.c
	@echo -e "${_GREEN}[C]${_END}   Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@ $(ERROR_FILTER)

#---------------------------------------------------------------------------------
%.o: %.S
	@echo  -e "${_RED}[ASM]${_END} Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)
#---------------------------------------------------------------------------------
%.png.o : %.png
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.jpg.o : %.jpg
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.ttf.o : %.ttf
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.bin.o : %.bin
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.wav.o : %.wav
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.mp3.o : %.mp3
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

#---------------------------------------------------------------------------------
%.ogg.o : %.ogg
	@echo $(notdir $<)
	@bin2s -a 32 $< | $(AS) -o $(@)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
