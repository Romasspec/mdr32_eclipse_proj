MCU=cortex-m3
OBJCOPY=arm-none-eabi-objcopy
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
SIZE=arm-none-eabi-size
INC  = -ICMSIS/device
INC += -ICMSIS/core
INC += -ISPL/inc
DEF = -DSTM32F10X_MD
CFLAGS=-mthumb -mcpu=$(MCU) -g -O0 -Wall $(DEF) $(INC)
ASFLAGS=-mthumb -mcpu=$(MCU) -g -Wall
LDFLAGS=-Tscript.ld
OBJ=main.o  init.o
TARGET=blink
.PHONY: all clean

%.o:	%.c
	$(CC) -c -o $@ $< $(CFLAGS)
%.o:	asm/%.s
	$(CC) -c -o $@ $< $(ASFLAGS)
all:	$(OBJ)
	$(LD) $(LDFLAGS) -g  -o $(TARGET).elf  $(OBJ)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	$(SIZE)  $(TARGET).elf
install:
	st-flash  write $(TARGET).bin 0x08000000
clean:
	@rm -v $(TARGET).elf $(TARGET).bin $(OBJ)
