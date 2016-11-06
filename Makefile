TARGET = main.nds
CC = $(DEVKITARM)/bin/arm-none-eabi-gcc
ARCH = -mthumb -mthumb-interwork
CFLAGS = -g -Wall -O2 -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math $(ARCH) -DARM9
NDSTOOL = $(DEVKITARM)/bin/ndstool
NDSEMU = /usr/local/bin/desmume-cli
GAME_ICON = $(DEVKITPRO)/libnds/icon.bmp
GAME_TITLE = COLLADA Model Viewer

.PHONY: run clean

$(TARGET): main.o main.elf
	$(NDSTOOL) -c $(TARGET) -9 main.elf -b $(GAME_ICON) "$(GAME_TITLE)" -d models

main.elf: main.o simplexml.o
	$(CC) -specs=ds_arm9.specs -g $(ARCH) main.o simplexml.o -L$(DEVKITPRO)/libnds/lib -lfilesystem -lfat -lnds9 -o main.elf

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -I$(DEVKITPRO)/libnds/include

simplexml.o: simplexml.c simplexml.h
	$(CC) $(CFLAGS) -c simplexml.c

run:
	$(NDSEMU) $(TARGET)

clean:
	rm *.o *.elf *.nds 

