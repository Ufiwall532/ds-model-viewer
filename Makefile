NAME = main
TARGET = $(NAME).nds
CC = $(DEVKITARM)/bin/arm-none-eabi-gcc
ARCH = -mthumb -mthumb-interwork
CFLAGS = -g -Wall -O2 -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math $(ARCH) -DARM9
NDSTOOL = $(DEVKITARM)/bin/ndstool
NDSEMU = /usr/local/bin/desmume-cli
GAME_ICON = $(DEVKITPRO)/libnds/icon.bmp
GAME_TITLE = Hello by Caleb

.PHONY: run clean

$(TARGET): $(NAME).o $(NAME).elf
	$(NDSTOOL) -c $(TARGET) -9 $(NAME).elf -b $(GAME_ICON) "$(GAME_TITLE)"

main.o: $(NAME).c
	$(CC) -MMD -MP -MF $(NAME).d $(CFLAGS) -c $(NAME).c -I$(DEVKITPRO)/libnds/include -o $(NAME).o

main.elf: $(NAME).o
	$(CC) -specs=ds_arm9.specs -g $(ARCH) $(NAME).o -L$(DEVKITPRO)/libnds/lib -lnds9 -o $(NAME).elf

run:
	$(NDSEMU) $(TARGET)

clean:
	rm *.o *.d *.elf *.nds 


