# This is a minimal set of ANSI/VT100 color codes
_END=$'\033[0m
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

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(filter $(DEVKITARM)/bin,$(PATH)),)
export PATH:=$(DEVKITARM)/bin:$(PATH)
endif

CC = arm-none-eabi-gcc
# LINK = arm-none-eabi-gcc
LINK = arm-none-eabi-ld
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
CFLAGS += -Wall -mbig-endian -std=gnu99 -mcpu=arm926ej-s -Os -I$(DEVKITPRO)/libnds/include
LDFLAGS += --script=link.ld -EB -L"$(DEVKITARM)/arm-none-eabi/lib" -Map=output.map

CFILES = $(wildcard source/*.c)
BINFILES = $(wildcard data/*.bin)
OFILES = $(BINFILES:data/%.bin=build/%.bin.o)
OFILES += $(CFILES:source/%.c=build/%.o)
DFILES = $(CFILES:source/%.c=build/%.d)
SFILES = $(wildcard source/*.s)
OFILES += $(SFILES:source/%.s=build/%.o)
PROJECTNAME = ${shell basename "$(CURDIR)"}
CWD = "$(CURDIR)""

#---------------------------------------------------------------------------------
# canned command sequence for binary data, taken from devkitARM
#---------------------------------------------------------------------------------
define bin2o
	bin2s $< | $(AS) -o $(@)
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > source/`(echo $(<F) | tr . _)`.h
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> source/`(echo $(<F) | tr . _)`.h
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> source/`(echo $(<F) | tr . _)`.h
endef

.PHONY:=all dirs

all: dirs $(PROJECTNAME).bin $(PROJECTNAME)_bin.h

dirs:
	@mkdir -p build

$(PROJECTNAME).bin: $(PROJECTNAME).elf
#	$(OBJCOPY) -O binary $< $@
	@$(OBJCOPY) -j .text -j .rodata -O binary $< $@

$(PROJECTNAME)_bin.h: $(PROJECTNAME).bin
	@echo -e "${_PURPLE}[ARM XXD]${_END} Creating ${_CYAN}$(notdir $@)${_END} from ${_CYAN}$(notdir $<)${_END}.."
	@xxd -i $< | sed "s/unsigned/static const unsigned/g;s/$(PROJECTNAME)$*/$(PROJECTNAME)/g" > $@
	@cp $(PROJECTNAME)_bin.h ../../source/iosu_payload/$(PROJECTNAME).h

$(PROJECTNAME).elf: show_head $(OFILES)
	@echo -e "${_PURPLE}[ARM LD]${_END}  Linking ${_CYAN}$(notdir $@)${_END}.."
	@$(LINK) $(LDFLAGS) -o $(PROJECTNAME).elf $(filter-out build/crt0.o, $(OFILES)) -L"libs" -lgcc


clean:
	@rm -f build/*.o build/*.d
	@rm -f $(PROJECTNAME).elf $(PROJECTNAME).bin $(PROJECTNAME)_bin.h

-include $(DFILES)

show_head:
	@echo -e "${_YELLOW}\nBuilding ARM user code${_END}"

build/%.o: source/%.c
	@echo -e "${_GREEN}[ARM C++]${_END} Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM $< > build/$*.d

build/%.o: source/%.s
	@echo -e "${_RED}[ARM ASM]${_END} Building ${_CYAN}$(notdir $<)${_END}.."
	@$(CC) $(CFLAGS) -xassembler-with-cpp -c $< -o $@
	@$(CC) -MM $< > build/$*.d

build/%.bin.o: data/%.bin
	@echo $(notdir $<)
	@$(bin2o)
