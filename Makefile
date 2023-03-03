##############################################################################
##
##  Makefile with automatic dependencies generation
##
##  (C) 2008, Alexey Presniakov (me@klad.me)
##
##############################################################################

SOURCES+= \
	src/stmlib/cortexm3_macro.s		\
	src/stmlib/stm32f10x_gpio.c		\
	src/stmlib/stm32f10x_iwdg.c		\
	src/stmlib/stm32f10x_nvic.c		\
	src/stmlib/stm32f10x_rcc.c		\
	src/stmlib/stm32f10x_systick.c	\
	src/stmlib/stm32f10x_usart.c	\
	src/stmlib/stm32f10x_dma.c		\
	src/stmlib/stm32f10x_can.c		\
	src/stmlib/c_startup.c

SOURCES+= \
	src/hw/startup.c				\
	src/hw/timer.c					\
	src/hw/uart.c					\
	src/hw/can.c					\
	src/hw/leds.c					\
	src/hw/wdt.c

SOURCES+= \
	src/main.c

##############################################################################



##############################################################################
##  Directories and file names
##############################################################################
# Output file name
OUT=usb2can
# Path for source files
SRCDIR=src
# Path for binary files
OUTDIR=out
# Path for object files
OBJDIR=.obj
# Path for dependencies information
DEPDIR=.dep
##############################################################################


##############################################################################
##  Includes & Defines
##############################################################################
DEFINES=-DSTM32F103C8
INCLUDES=-Isrc/ -Isrc/hw/ -Isrc/stmlib/
##############################################################################


##############################################################################
##  Compiler information
##############################################################################
CC=arm-none-eabi-gcc
CPP=arm-none-eabi-g++
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-g -mcpu=cortex-m3 -mthumb -Wall -Wstrict-prototypes -Wshadow -O3 \
	-fmerge-all-constants -fno-inline -ffunction-sections -fdata-sections \
	-fno-tree-scev-cprop -fno-split-wide-types \
	-fno-common -fno-builtin $(INCLUDES) $(DEFINES)
CPPFLAGS=-g -mcpu=cortex-m3 -mthumb -Wall -Wshadow -O3 \
	-fmerge-all-constants -fno-inline -ffunction-sections -fdata-sections \
	-fno-tree-scev-cprop -fno-split-wide-types \
	-fno-common -fno-builtin -nostdinc++ -fno-rtti -fno-exceptions $(INCLUDES) $(DEFINES)
LDFLAGS=-lm -Tsrc/stmlib/stm32f103c8.ld -nostartfiles -Wl,--relax,--gc-sections
##############################################################################



# Target ALL
all: $(OUTDIR)/$(OUT).bin


# Target for making Binary
$(OUTDIR)/$(OUT).bin: $(OUTDIR)/$(OUT).elf.cm3
	@echo "Making Binary Firmware..."; \
	$(OBJCOPY) -O binary $< $@

# Target for linker
$(OUTDIR)/$(OUT).elf.cm3: $(SOURCES:%=$(OBJDIR)/%.o)
	@echo "Linking..."; \
	$(CPP) $(CPPFLAGS) -o $@ -Wl,-Map,$(OUTDIR)/$(OUT).map $+ $(LDFLAGS)


# Target for burning
burn: $(OUTDIR)/$(OUT).bin
	@stm32flash -w $< /dev/ttyUSB0


# Target for clean
clean:
	rm -f $(OUTDIR)/$(OUT).elf.cm3 $(OUTDIR)/$(OUT).map
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)

# Target for distclean
distclean:
	rm -f $(OUTDIR)/$(OUT).elf.cm3 $(OUTDIR)/$(OUT).map $(OUTDIR)/$(OUT).bin
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)


# PHONY
.PHONY: all


# Rule for compiling ASM files
$(OBJDIR)/%.s.o: %.s
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CC) $(CFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$<.o $<

# Rule for compiling C files
$(OBJDIR)/%.c.o: %.c
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CC) $(CFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$<.o $<

# Rule for compiling C++ files
$(OBJDIR)/%.cpp.o: %.cpp
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CPP) $(CPPFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CPP) $(CPPFLAGS) -c -o $@ $<


# Rule for resource
$(OBJDIR)/%.o: %
	@echo "Embedding $<"; \
	ld -r -b binary -o $@ $<


# Including dependencies infomation
-include $(SOURCES:%=$(DEPDIR)/%.d)
