OUTPUT_FORMAT(binary)

SECTIONS
{
	. = 0x7c00;
	.text :
	{
		*(.mbr)
		*(.kentry)
		*(.text)
	}
	.data :
	{
		*(.rodata)
		*(.data)
		*(COMMON)
		*(.bss)
	}
}
