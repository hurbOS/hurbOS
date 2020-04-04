#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Welcome to hurbOS\n");
	init_gdt_tables();
}