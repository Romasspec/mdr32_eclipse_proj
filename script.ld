/* Highest address of the user mode stack */
_estack = 0x20005000;    /* end of RAM */

/* Memories definition */
MEMORY
{
  RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 20K
  ROM (rx)      : ORIGIN = 0x8001000, LENGTH = 128K
}

/* Sections */
SECTIONS
{
  /* The program code and other data into ROM memory */
	.text :
  {
   *(.vectors)           /* .text sections (code) */
    KEEP(*(.vectors))
    . = ALIGN(4); 
    *(.text)           /* .text sections (code) */    
  } >ROM

  /* Constant data into ROM memory*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
  } >ROM

	_sidata = LOADADDR(.data);

  /* Initialized data sections into RAM memory */
  .data :
  {
  	_sdata = . ;        	/* STM specific definition */
    __data_start__ = . ;
    
    . = ALIGN(4);
    *(.data)           /* .data sections */
    
    _edata = . ;        	/* STM specific definition */
    __data_end__ = . ;
  } >RAM AT> ROM


  /* Uninitialized data section into RAM memory */
  . = ALIGN(4);
  .bss :
  {
	__bss_start__ = .;     	/* standard newlib definition */
    _sbss = .;              /* STM specific definition */
    *(.bss)
    *(COMMON)

    . = ALIGN(4);
    __bss_end__ = .;        /* standard newlib definition */
    _ebss = . ;             /* STM specific definition */
  } >RAM

}