.text
.globl _start

.code16
_start:
	# load kernel at 0x7e00
	movb $2, %ah
	movb $63, %al
	movb $2, %cl
	movb $0, %ch
	movb $0, %dh
	mov $0x7e00, %bx
	int $0x13
	
	# set video mode to textmode
	mov $0, %ah
	mov $3, %al
	int $0x10
	
	# load gdt
	cli
	lgdt gdt_descriptor
	
	# enter protected mode
	mov %cr0, %eax
	or $1, %eax
	mov %eax, %cr0
	
	# jump into 32-bit code
	ljmp $(gdt_code - gdt_start), $_setup32

.code32
_setup32:
	# setup stack
	mov $(gdt_data - gdt_start), %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov $0x90000, %ebp
	mov %ebp, %esp
	
	# jump into kernel at 0x7e00
	jmp 0x7e00

gdt_start:
	# null
	.long 0x0
	.long 0x0
gdt_code:
	.word 0xffff
	.word 0
	.byte 0
	.byte 0b10011010
	.byte 0b11001111
	.byte 0
gdt_data:
	.word 0xffff
	.word 0
	.byte 0
	.byte 0b10010010
	.byte 0b11001111
	.byte 0
gdt_end:
gdt_descriptor:
	.word gdt_end - gdt_start - 1
	.long gdt_start

. = _start + 510
.byte 0x55, 0xAA
