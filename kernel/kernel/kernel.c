#include <stdio.h>
#include "kernel/tty.h"
#include "kernel/gdt.h"
#include <stdlib.h>

void kernel_main(void) {
	terminal_initialize();
	printf("[O] Terminal Initialized\n");
	gdt_flush();
	printf("[O] GDT Initialized\n");
}
