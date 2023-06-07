AS = i686-elf-as
CC = i686-elf-gcc -ffreestanding
LD = i686-elf-ld

CFLAGS = -Os -Wall
ASFLAGS = -Os
LDFLAGS = -flto -s

INC = -I include -I arch/x86/include

all:
	mkdir -p build
	$(AS) $(ASFLAGS) arch/x86/boot.s -o build/boot.o
	$(CC) $(CFLAGS) kern/kmain.c -c -o build/kmain.o -I include -I arch/x86/include
	$(CC) $(CFLAGS) dev/textmode.c -c -o build/textmode.o -I include -I arch/x86/include
	$(CC) $(CFLAGS) dev/ps2.c -c -o build/ps2.o -I include -I arch/x86/include
	$(CC) $(CFLAGS) klibc/itoa.c -c -o build/itoa.o -I include -I arch/x86/include
	$(LD) $(LDFLAGS) -T linker.ld build/boot.o build/kmain.o build/textmode.o build/itoa.o build/ps2.o -o build/kernel.bin
	
	dd if=/dev/zero of=hd.img bs=512 count=128
	dd if=build/kernel.bin of=hd.img conv=notrunc
