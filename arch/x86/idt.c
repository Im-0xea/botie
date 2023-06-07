#include <stdint.h>

struct idt_e {
	uint16_t basel;
	uint16_t segment;
	uint8_t  reserved;
	uint8_t  flags;
	uint8_t  baseh;
}

struct idt_d {
	uint16_t           limit;
	struct idt_entry * entry;
}

struct idt_e idt[256];

struct idt_d idt_descriptor = {
	sizeof(idt) -1,
	idt
};
