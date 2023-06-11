#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <itoa.h>
#include <asm/io.h>
#include <textmode.h>

#if !defined(VGA_BLINKING)
#define VGA_BLINKING 0
#endif

#if !defined(VGA_UNDERLINE)
#define VGA_UNDERLINE 0
#endif

static uint16_t *    textmode_address = (uint16_t *) 0xb8000;
static const uint8_t
	textmode_width  = 80,
	textmode_height = 25,
	textmode_tab    = 8;

static uint8_t
	c_x = 0,
	c_y = 0,
	c_atr = 0x0f;

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
		}
		break;
	case '\t':
		const uint8_t new_x = ((c_x / textmode_tab) + 1) * textmode_tab;
		if (new_x < textmode_width) {
			c_x = new_x;
		} else {
			c_x = 0;
			shift();
		}
		break;
	default:
		textmode_address[pos] = vga_entry(c, c_atr);
		++c_x;
	}
}
static void set_fg(const uint8_t col)
{
	c_atr = (c_atr & 0xf0) | col;
}

static void set_bg(const uint8_t col)
{
	c_atr = (c_atr & 0x0f) | (col << 4);
}
static void set_reversed(void)
{
	const uint8_t fg = c_atr & 0x0F;
	const uint8_t bg = (c_atr >> 4) & 0x0F;
	set_fg(bg);
	set_bg(fg);
}
static void read_num(const char ** cp, uint8_t * const num)
{
	while (1) {
		const char c = **cp;
		if (!c || c < '0' || c > '9')
			break;
		*num = (*num * 10) + (c - '0');
		++*cp;
	}
}
static uint8_t ansi_color(const uint8_t ansi)
{
	switch (ansi) {
	case 0: return 0;
	case 1: return 4;
	case 2: return 2;
	case 3: return 6;
	case 4: return 1;
	case 5: return 5;
	case 6: return 3;
	case 7: return 7;
	default: return 0;
	}
}

static void ansi_interpreter(const char ** cp)
{
	uint8_t cou = 0;
	uint8_t sec = 0;
	
	read_num(cp, &cou);
	retry:
	switch (**cp) {
	case 'A':
		if (cou < c_y) {
			cou = c_y;
		}
		c_y -= cou;
		break;
	case 'B':
		c_y += cou;
		break;
	case 'C':
		c_x += cou;
		break;
	case 'D':
		c_x -= cou;
		break;
	case 'E':
		c_x = 0;
		c_y += cou;
		break;
	case 'F':
		c_x = 0;
		c_y -= cou;
		break;
	case 'G':
		c_x = cou;
		break;
	case ';':
		++*cp;
		sec = cou;
		read_num(cp, &cou);
		const char c = **cp;
		if (c == 'H' || c == 'f') {
			c_x = sec;
			c_y = cou;
		}
		goto retry;
		break;
	case 'J':
		switch (cou) {
		case 0:
			//txt_set((vga_w * c_y) + c_x, vga_h * vga_w, NULL);
			break;
		case 1:
			//txt_set(0, (vga_w * c_y) + c_x, NULL);
			break;
		case 2:
		case 3:
			//txt_set(0, vga_w * vga_h, NULL);
			break;
		}
		break;
	case 'K':
		switch (cou) {
		case 0:
			//txt_set((vga_w * c_y) + c_x, vga_w * (c_y + 1), NULL);
			break;
		case 1:
			//txt_set(vga_w * c_y, (vga_w * c_y) + c_x, NULL);
			break;
		}
		break;
	case 'S':
	case 'T':
		// undefined - this driver does not have a scrollback buffer
		break;
	case 'm':
		if (!cou)
			c_atr = 0x07;
		else if (cou == 1) /* bold */
			c_atr |= (1 << 3);
#if VGA_UNDERLINE
		else if (cou == 4) /* underline */
			c_atr |= 1;
#endif
#if VGA_BLINKING
		else if (cou == 5) /* blinking */
			c_atr |= (1 << 7);
#endif
		else if (cou == 7)
			set_reversed();
		else if (cou == 22) /* unbold */
			c_atr &= 0b11110111;
		else if (cou >= 30 && cou <= 37)
			set_fg(ansi_color(cou - 30));
		else if (cou >= 90 && cou <= 97)
			set_fg(8 + ansi_color(cou - 90));
		else if (cou >= 40 && cou <= 47)
			set_bg(ansi_color(cou - 40));
		else if (cou >= 100 && cou <= 107)
			set_bg(8 + ansi_color(cou - 100));
		break;
	case 'n':
		if (cou == 6) {
			kprintf("^[[%d;%dR", c_y, c_x);
		}
		break;
	}
	++*cp;
}
static void escape(const char ** ptr)
{
	const int b = *++*ptr;
	if (!b || b != '[')
		return;
	
	++*ptr;
	ansi_interpreter(ptr);
}

static void puts(const char * str)
{
	while (1) {
		const char c = *str;
		if (!c)
			break;
		else if (c == '\033') {
			escape(&str);
			continue;
		}
		
		putc(c);
		++str;
	}
}

static void vprintf(const char * fmt, va_list args)
{
	char buf[64];
	while (1) {
		const char c = *fmt;
		if (!c)
			break;
		
		if (c == '\033') {
			escape(&fmt);
			continue;
		} else if (c != '%') {
			putc(c);
			++fmt;
			continue;
		}
		
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
		case 'd':
			itoa(va_arg(args, const int), buf, 10);
			puts(buf);
			break;
		}
	}
}

void kputc(const char c)
{
	putc(c);
	set_cur();
}
void kputs(const char * const str)
{
	puts(str);
	set_cur();
}
void kprintf(const char * const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	set_cur();
}
