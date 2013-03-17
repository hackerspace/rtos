MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K
/*  MEMORY_B1 (rx) : ORIGIN = 0x60000000, LENGTH = 0K*/
}

ENTRY(_start)
SECTIONS
{

  . = 0x0;
	.text : {
/*    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code  */
    . = ALIGN(4);

		*(.reset)
		*(.text)
	} > FLASH
	

	. = 0x20000000;
	__exidx_start = .;
	.ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
	__exidx_end = .;


	.data : { *(.data) }
	__bss_start__ = .;
	.bss : { *(.bss) }
	__bss_end__ = .;
	
	
}
