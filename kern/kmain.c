#include <stdint.h>
#include <textmode.h>
#include <ps2.h>

int kmain(void) __attribute__((section(".kentry")));

void setup_interrupt(void (*handler)())
{
	
}
int kmain(void) {
	kprintf("Bootie Boot >:3\nComplete :O");
	
	setup_interrupt(keyboard_interrupt);
	
	while (1);
}
