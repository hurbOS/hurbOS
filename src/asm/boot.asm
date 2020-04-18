MBALIGN  equ 1<<0
MEMINFO  equ 1<<1
FB    	 equ 1<<2
FLAGS    equ MBALIGN | MEMINFO | FB
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
	align 4
		dd MAGIC
		dd FLAGS
		dd CHECKSUM
		times 6 dd 0
		dd 1280
		dd 720
		dd 32

KNL_HIGH_VMA equ 0xFFFFFFFF80000000
section .rodata
	align 16
		gdt_ptr:
			dw gdt_end - gdt_start - 1
			dq gdt_start - KNL_HIGH_VMA

	align 16
		gdt_higherhalf:
			dw gdt_end - gdt_start - 1
			dq gdt_start

	align 16
	gdt_start:
		.null_descriptor:
			dw 0x0000
			dw 0x0000
			db 0x00
			db 00000000b
			db 00000000b
			db 0x00

		.kernel_code_64:
			dw 0x0000
			dw 0x0000
			db 0x00
			db 10011010b
			db 00100000b
			db 0x00

		.kernel_data:
			dw 0x0000
			dw 0x0000
			db 0x00
			db 10010010b
			db 00000000b
			db 0x00

		.user_data_64:
			dw 0x0000
			dw 0x0000
			db 0x00
			db 11110010b
			db 00000000b
			db 0x00
		.user_code_64:
			dw 0x0000
			dw 0x0000
			db 0x00
			db 11111010b
			db 00100000b
			db 0x00
		.tss_64:
			dq 0x0
			dq 0x0

	align 16
	gdt_end:

section .data
	align 4096
	boot_pml2:
		dq 0 + 0x83
		dq 0x200000 + 0x83
		dq 0x400000 + 0x83
		dq 0x600000 + 0x83
		dq 0x800000 + 0x83
		dq 0xa00000 + 0x83
		dq 0xc00000 + 0x83
		dq 0xe00000 + 0x83
		dq 0x1000000 + 0x83
		dq 0x1200000 + 0x83
		dq 0x1400000 + 0x83
		dq 0x1600000 + 0x83
		dq 0x1800000 + 0x83
		dq 0x1a00000 + 0x83
		dq 0x1c00000 + 0x83
		dq 0x1e00000 + 0x83
		dq 0x2000000 + 0x83
		times 495 dq 0

	boot_pml3:
		dq boot_pml2 - KNL_HIGH_VMA + 0x3
		times 509 dq 0
		dq boot_pml2 - KNL_HIGH_VMA + 0x3
		dq 0
	
	boot_pml4:
		dq boot_pml3 - KNL_HIGH_VMA + 0x3
		times 255 dq 0
		dq boot_pml3 - KNL_HIGH_VMA + 0x3
		times 254 dq 0
		dq boot_pml3 - KNL_HIGH_VMA + 0x3

section .bss
	stack_bottom:
		resb 16384
	stack_top:

section .text
	[bits 32]
	[global _start]
	_start:
		cli
		lgdt [gdt_ptr - KNL_HIGH_VMA]

		mov esp, stack_top - KNL_HIGH_VMA

		mov edi, eax
		mov esi, ebx

		mov eax, cr4
		or eax, 0x000000A0
		mov cr4, eax

		mov eax, boot_pml4 - KNL_HIGH_VMA
		mov cr3, eax

		mov ecx, 0xC0000080
		rdmsr
		or eax, 0x00000901
		wrmsr

		mov eax, cr0
		or eax, 0x80000001
		mov cr0, eax

		jmp 0x08:higher_half_entry - KNL_HIGH_VMA

	[bits 64]
	higher_half_entry:
		mov rax, higher_half
		jmp rax

	[extern _start64]
	higher_half:
		lgdt [gdt_higherhalf]

		mov ax, 0x0
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax

		jmp _start64

