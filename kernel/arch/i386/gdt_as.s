.global gdt_flush

gdtr DW 0
     DD 0

gdt_flush:
   mov 4(%esp), %eax
   lgdt (%eax)

   mov 0x10, %ax    
   mov %ax, %ds      
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   ret