#include <stdio.h>
#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Welcome to hurbOS");
}
