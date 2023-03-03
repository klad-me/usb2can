GROUP(libgcc.a libc.a)

MEMORY
{
	 FLASH (rx) : ORIGIN = 0k LENGTH = 64K
	 SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_vRamTop = 0x20000000 + 20K;

ENTRY(ResetISR)

SECTIONS
{
		.text :
		{
				KEEP(*(.isr_vector))
				KEEP(*(.cfg_version))
				*(.text*)
				*(.rodata*)

		} > FLASH


		/* for exception handling/unwind - some Newlib functions (in common with C++ and STDC++) use this. */
		
		.ARM.extab : 
		{
				*(.ARM.extab* .gnu.linkonce.armextab.*)
		} > FLASH

		__exidx_start = .;
		.ARM.exidx :
		{
				*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		} > FLASH
		__exidx_end = .;
		__free_flash_start = .;

		_etext = .;
				
		.data : AT (__exidx_end)
		{
				_data = .;
				*(vtable)
				*(.data*)
				_edata = .;
		} > SRAM

		/* zero initialized data */
		.bss :
		{
				_bss = .;
				*(.bss*)
				*(COMMON)
				_ebss = .;
		} > SRAM
		
		/* Where we put the heap with cr_clib */
		.cr_heap :
		{
				end = .;
				_pvHeapStart = .;
		} > SRAM

/*
		Note: (ref: M0000066)
		Moving the stack down by 16 is to work around a GDB bug.
		This space can be reclaimed for Production Builds.
*/		
		_vStackTop = _vRamTop - 16;
}
