#include <stdint.h>
#include <stdbool.h>
#include <asm/io.h>
#include <textmode.h>

static int key_flags;
int        key_store = 0;

char keymap[] =
"\e1234567890  \b ',.pyfgcrl  \n\033aoeuidhtns  X ;qjkxbmwvz    "
"\e!@#$%^&*()  \b \"<>PYFGCRL  \n\033AOEUIDHTNS  X :QJKXBMWVZ    ";

static inline void
toggle_flag(const uint8_t n)
{
	key_flags ^= 0b00000001 << n;
}

static inline bool
check_flag(const uint8_t n)
{
	return key_flags & 0b00000001 << n;
}

static char decode_ps2(const uint8_t code)
{
	if (!code)
		return '\0';
	
	switch (code) {
	case 42:
	case 52:
	case 170:
	case 182:
		toggle_flag(0); /* shift */
		break;
	case 29:
	case 157:
		toggle_flag(2); /* ctrl */
		break;
	default:
		if (code > 60)
			break;
		
		return check_flag(1) ?
			keymap[code + (sizeof(keymap) / 2)] :
			keymap[code];
	}
	
	return '\0';
}

void __attribute__((interrupt)) keyboard_interrupt(struct interrupt_frame * frame)
{
	const uint8_t scan = inb(0x60);
	key_store = decode_ps2(scan);
	outb(0x20, 0x20);
}
