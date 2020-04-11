.global gdt_flush
.global reload_segments
.intel_syntax noprefix

.EXTERN gdtptr

.section .text
gdt_flush:
   lgdt  [gdtptr]
	jmp   0x08:reload_CS

.att_syntax
reload_CS:
   mov   0x10, %ax
   mov   %ax, %ds
   mov   %ax, %es
   mov   %ax, %fs
   mov   %ax, %gs
   mov   %ax, %ss
   ret
