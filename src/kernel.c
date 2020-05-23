#include <system.h>

void kmain(uint64_t magic, multiboot_info_t* info) {
    pmm_init(info);
    vmm_init();
    while (1) {
        asm volatile("hlt");
    }
}

