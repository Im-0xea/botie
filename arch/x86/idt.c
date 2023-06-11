#include <stdint.h>
#include <stdbool.h>

struct __attribute__((packed)) idt_e {
	uint16_t basel;
	uint16_t segment;
	uint8_t  reserved;
	uint8_t  flags;
	uint8_t  baseh;
};

struct __attribute__((packed)) idt_d {
	uint16_t           limit;
	struct idt_e * entry;
};

struct idt_e __attribute__((aligned(0x10))) idt[256];

struct idt_d idt_descriptor = {
	sizeof(idt) -1,
	idt
};

void __attribute__((noreturn)) exception_handler()
{
	kputs("uuuuuh");
	__asm__ volatile ("cli; hlt");
	while (1);
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags)
{
	struct idt_e * descriptor = &idt[vector];

	descriptor->basel    = (uint32_t)isr & 0xFFFF;
	descriptor->segment  = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
	descriptor->flags    = flags;
	descriptor->baseh    = (uint32_t)isr >> 16;
	descriptor->reserved = 0;
}

void idt_init(void)
{
	asm volatile ("cli");
	for (uint8_t vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, exception_handler, 0x8E);
	}
	asm volatile ("lidt %0" : : "m"(idt_descriptor));
	asm volatile ("sti");
}
