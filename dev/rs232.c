int rs232_init(const uint16_t port)
{
	outb(port + 1, 0x00); /* disable int */
	outb(port + 3, 0x80); /* DLAB */
	outb(port,     0x03); /* set 38400 baud */
	outb(port + 1, 0x00); /* high byte */
	outb(port + 1, 0x00); /* high byte */
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
	outb(port + 4, 0x1E);
	outb(port + 0, 0xAE);
	
	if(inb(PORT + 0) != 0xAE) {
		return 1;
	}
	
	outb(PORT + 4, 0x0F);
	return 0;
}
