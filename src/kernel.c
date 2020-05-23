<<<<<<< HEAD
#include <system.h>

void kmain(uint64_t magic, multiboot_info_t* info) {
    pmm_init(info);
    vmm_init();
    while (1) {
        asm volatile("hlt");
    }
}

=======
// kernel.c
// Aurelian, MaxiMal_98

//#include <stdio.h>
//#include <splash.h>

void kmain(void) {
}
>>>>>>> 5ec5da79c5b66690213e739ccb78661b8d2544f4
