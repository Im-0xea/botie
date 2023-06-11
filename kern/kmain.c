#include <stdint.h>
#include <textmode.h>
#include <ps2.h>

int kmain(void) __attribute__((section(".kentry")));

void setup_interrupt(void (*handler)())
{
	
}
int kmain(void) {
	idt_init();
	kprintf("\033[31mRed\033[0m\t\033[32mGreen\033[0m\n"
		"\033[33mYellow\033[0m\t\033[34mBlue\033[0m\n"
		"\033[35mMagenta\033[0m\t\033[36mCyan\033[0m\n"
		"\033[37mWhite\033[0m\t\033[90mGray\033[0m\n"
		"\033[91mBright Red\033[0m\t\033[0;92mBright Green\033[0m\n"
		"\033[93mBright Yellow\033[0m\t\033[0;94mBright Blue\033[0m\n"
		"\033[95mBright Magenta\033[0m\t\033[0;96mBright Cyan\033[0m\n"
		"\033[97mBright White\033[0m\n"
		"\033[40mBlack Background\033[0m\t\033[41mRed Background\033[0m\n"
		"\033[42mGreen Background\033[0m\t\033[43mYellow Background\033[0m\n"
		"\033[44mBlue Background\033[0m\t\t\033[45mMagenta Background\033[0m\n"
		"\033[46mCyan Background\033[0m\t\t\033[47mWhite Background\033[0m\n"
		"\033[1mBold Text\033[0m\t\033[3mItalic Text\033[0m\n"
		"\033[4mUnderline Text\033[0m\t\033[9mCrossed-out Text\033[0m\n"
		"\033[5mBlinking Text\033[0m\t\033[7mReversed Text\033[0m\n\n > ");
	
	asm volatile("hlt");
	while (1);
}
