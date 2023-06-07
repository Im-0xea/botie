#include <stdint.h>
#include <stdarg.h>
#include <asm/io.h>

static uint16_t *       textmode_address = (uint16_t *) 0xb8000;
static const uint8_t    textmode_width   = 80, textmode_height  = 25;
static const uint8_t    textmode_tab     = 8;

static uint8_t          c_x = 0, c_y = 0, c_atr = 0x0f;

static inline uint16_t vga_entry(const unsigned char c, const uint8_t a)
{
	return c | a << 8;
}

static void def_cur(const uint8_t cur_start, const uint8_t cur_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cur_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cur_end);
}

static void set_cur()
{
	const uint16_t pos = c_y * textmode_width + c_x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void shift(void)
{
	if (c_y + 1 == textmode_height) {
		
	} else {
		++c_y;
	}
	c_x = 0;
	set_cur();
}

static void putc(const char c)
{
	const uint16_t pos = c_y * textmode_width + c_x;
	switch (c) {
	case '\r':
		c_x = 0;
		set_cur();
		break;
	case '\n':
		shift();
		break;
	case '\b':
		if (c_x) {
			--c_x;
			textmode_address[pos - 1] = vga_entry(' ', c_atr);
			set_cur();
		}
		break;
	case '\t':
		if (c_x + textmode_tab < textmode_width) {
			c_x += textmode_tab;
			set_cur();
		} else {
			shift();
		}
		break;
	default:
		textmode_address[pos] = vga_entry(c, c_atr);
		++c_x;
	}
}
static void puts(char * str)
{
	while (1) {
		const char c = *str;
		if (!c)
			break;
		
		putc(c);
		++str;
	}
}

static void vprintf(const char * fmt, va_list args)
{
	while (1) {
		const char c = *fmt;
		if (!c)
			break;
		
		if (c == '%') {
			++fmt;
			const char fc = *fmt;
			switch (fc) {
			case '%':
				putc('%');
				++fmt;
				break;
			case 's':
				puts(va_arg(args, char *));
				break;
			}
		} else {
			putc(c);
			++fmt;
		}
	}
	set_cur();
}

void kputc(const char c)
{
	putc(c);
	set_cur();
}
void kputs(char * str)
{
	puts(str);
	set_cur();
}
void kprintf(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
}
