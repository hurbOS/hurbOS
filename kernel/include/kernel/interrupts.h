#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

struct IDT_entry{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

struct IDT_entry IDT[256];

#endif