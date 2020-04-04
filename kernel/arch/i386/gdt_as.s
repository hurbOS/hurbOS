.global gdt_flush
.global reload_segments
.intel_syntax noprefix

.section .text
gdt_flush:
   mov   eax, [esp+4]
   mov   [gdtr+2], eax
   mov   ax, [esp+8]
   mov   [gdtr], ax
   lgdt  [gdtr]
   ret
   
reload_segments:
   jmp   0x08:reload_CS

reload_CS:
   mov   ax, 0x10
   mov   ds, ax
   mov   es, ax
   mov   fs, ax
   mov   gs, ax
   mov   ss, ax
   ret

.att_syntax
.section .data
gdtr: .word 0
      .double 0