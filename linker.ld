ENTRY (_start)

OUTPUT_FORMAT(elf64-x86-64)

KNL_HIGH_VMA = 0xFFFFFFFF80000000;
KNL_BOOT_VMA = 0x100000;

SECTIONS
{ 
   . = KNL_BOOT_VMA;
   .multiboot ALIGN(4K):
   {
      *(.multiboot)
   }

   . += KNL_HIGH_VMA;

   .text ALIGN(4K): AT(ADDR(.text) - KNL_HIGH_VMA)
   {
      *(.text)
   }

   .data ALIGN(4K): AT(ADDR(.data) - KNL_HIGH_VMA)
   {
	   *(.data)
      *(.rodata)
   }

   .bss ALIGN(4K): AT(ADDR(.bss) - KNL_HIGH_VMA)
   {
      *(COMMON)
	   *(.bss)
   }
   KNL_CORE_END = ALIGN(4K);
}
