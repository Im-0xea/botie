AS = i686-elf-as
CC = i686-elf-gcc
LD = i686-elf-ld

CFLAGS = -Os -Wall
ASFLAGS = -Os
LDFLAGS = -flto

INC = -I include -I arch/x86/include

all:
	mkdir -p build
	$(AS) $(ASFLAGS) arch/x86/boot/boot.s -o build/boot.o
	$(LD) $(LDFLAGS) -Ttext 0x7c00 --oformat=binary build/boot.o -o build/boot.bin
	
	$(CC) $(CFLAGS) kern/kernel.c -c -o build/kernel.o -m32 -ffreestanding -I include -I arch/x86/include
	$(CC) $(CFLAGS) dev/textmode.c -c -o build/textmode.o -m32 -ffreestanding -I include -I arch/x86/include
	$(LD) $(LDFLAGS) -Ttext 0x7e00 --oformat=binary build/kernel.o build/textmode.o -o build/kernel.bin
	
	cat build/boot.bin build/kernel.bin > build/os.bin
	dd if=/dev/zero of=hd.img bs=512 count=128
	dd if=build/os.bin of=hd.img conv=notrunc
