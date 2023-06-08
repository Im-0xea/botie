AS = i686-elf-as
CC = i686-elf-gcc -ffreestanding
LD = i686-elf-ld

CFLAGS = -Oz -Wall
ASFLAGS = -Oz
LDFLAGS = -flto -s

INC = -I include -I arch/x86/include

all:
	mkdir -p build
	$(AS) $(ASFLAGS) $(INC) arch/x86/boot.s -o build/boot.o
	$(CC) $(CFLAGS) $(INC) kern/kmain.c -c -o build/kmain.o
	$(CC) $(CFLAGS) $(INC) dev/textmode.c -c -o build/textmode.o
	$(CC) $(CFLAGS) $(INC) dev/ps2.c -c -o build/ps2.o
	$(CC) $(CFLAGS) $(INC) klibc/itoa.c -c -o build/itoa.o
	$(LD) $(LDFLAGS) -T linker.ld build/boot.o build/kmain.o build/textmode.o build/itoa.o build/ps2.o -o build/kernel.bin
	
	dd if=/dev/zero of=hd.img bs=512 count=128
	dd if=build/kernel.bin of=hd.img conv=notrunc
	du -b build/kernel.bin
