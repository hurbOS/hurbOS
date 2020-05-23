#include <mm/pmm.h>
// TODO: introduce locking functionality to prevent multiple cores from allocating memory at once

volatile static uint64_t total_mem = 0;

volatile static uint64_t total_alloc = 0;

pmm_bitmap memmap;

extern uint64_t KNL_CORE_END;

uint64_t pmm_totalalloc() {
    uint64_t ret = total_alloc;
    return ret;
}

uint64_t pmm_totalmem() {
    uint64_t ret = total_mem;
    return ret;
}

void pmm_init(multiboot_info_t* info) {
    multiboot_memory_map_t* current_entry = (multiboot_memory_map_t*) info->mmap_addr;

    while (current_entry < info->mmap_addr + info->mmap_length) {
        total_mem += current_entry->len;
        current_entry = (multiboot_memory_map_t *)((uintptr_t)current_entry + current_entry->size + sizeof (current_entry->size));
    }

    memmap = BITMAP_BASE + MEM_HIGH_OFFSET;

    current_entry = (multiboot_memory_map_t*) info->mmap_addr;

    while (current_entry < info->mmap_addr + info->mmap_length) {
        for (uint64_t i = current_entry->addr; i < current_entry->addr + current_entry->len; i+=PAGE_SIZE) {
            if (current_entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
                sbit(memmap, i / PAGE_SIZE);
            }
        }
    
        current_entry = (multiboot_memory_map_t *)((uintptr_t)current_entry + current_entry->size + sizeof (current_entry->size));
    }


    uint64_t kend = (uint64_t) &KNL_CORE_END - KERNEL_OFFSET;
    uint64_t len = (total_mem / PAGE_SIZE) / 8;
    
    for (uint64_t i = 1; i < ALIGN4K(kend) / PAGE_SIZE; i++) {
        sbit(memmap, i);
    }

    for (uint64_t i = 4096; i < len; i++) {
        sbit(memmap, i);
    }
}

void* pmm_malloc(uint64_t size) {
    if (size == PAGE_SIZE) {
        for (uint64_t i = 0; i < total_mem / PAGE_SIZE; i++) {
            if (!rbit(memmap, i)) {
                sbit(memmap, i);
                return (i * PAGE_SIZE) + MEM_HIGH_OFFSET;
            }
        }
    } else {
        uint64_t target = size / PAGE_SIZE;
        uint64_t i = 0;
        for (i = 0; i < total_mem / PAGE_SIZE;) {
            if (!rbit(memmap, i++)) {
                if (!--target) {
                    goto out;
                }
            } else {
                target = size / PAGE_SIZE;
            }
        }

        out:;
            target = size / PAGE_SIZE;
            uint64_t j = i - target;
            for (; j < i; j++) {
                sbit(memmap, j);
            }

            total_alloc += size;
            return (j * PAGE_SIZE) + MEM_HIGH_OFFSET;
    }
    return NULL;
}

void pmm_free(uint64_t ptr, uint64_t size) {
    total_alloc -= size;
    ptr -= MEM_HIGH_OFFSET;
    if (size == PAGE_SIZE) {
        cbit(memmap, ptr / PAGE_SIZE);
    } else {
        for (uint64_t i = ptr; i < size / PAGE_SIZE; i+=PAGE_SIZE) {
            cbit(memmap, i / PAGE_SIZE);
        }
    }
}