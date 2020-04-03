#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>

void kernel_main(void) {
	init_gdt_tables();
	terminal_initialize();
	printf("Welcome to hurbOS\n");
}
