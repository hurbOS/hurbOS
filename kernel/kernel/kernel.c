#include <stdio.h>
#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Carraige \r return test \n with a new line");
}
