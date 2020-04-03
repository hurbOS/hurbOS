.global gdt_flush

gdt_flush:
   mov (esp+4), %eax
   lgdt (eax)

   mov 0x10, %ax    
   mov %ax, %ds      
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   jmp 0x08:_flush 
.flush:
   ret