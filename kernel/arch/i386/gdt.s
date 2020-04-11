.global gdt_flush
.global gdt_init

.section .data
gdt_start:
gdt_null:
.long 0x0
.long 0x0

gdt_kernC:
.word 0xFFFF
.word 0x0
.byte 0x0
.byte 0b10011010
.byte 0b11001111
.byte 0x0

gdt_kernData:
.word 0xFFFF
.word 0x0
.byte 0x0
.byte 0b10010010
.byte 0b11001111
.byte 0x0
gdt_end:

gdtr:
.word (gdt_end - gdt_start - 1)
.long gdt_start

.section .text
gdt_flush:
	cli
	lgdt (gdtr)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x08, $flush_end

flush_end:
	ret
