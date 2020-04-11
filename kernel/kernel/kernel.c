#include <stdio.h>
#include "kernel/tty.h"
#include "kernel/gdt.h"
#include <stdlib.h>

void kernel_main(void) {
	init_gdt();
	terminal_initialize();
	printf("[O] Terminal Initialized\n");
}
