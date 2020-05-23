#ifndef PMM
#define PMM

#include <stddef.h>
#include <stdint.h>
#include <multiboot/multiboot.h>

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_2MB 0x200000
#define PAGE_SIZE_1MB 0x100000

#define MEM_HIGH_OFFSET 0xFFFF800000000000
#define BITMAP_BASE 0x1000000
#define KERNEL_OFFSET 0xFFFFFFFF80000000
#define ALIGN4K(x) (((uint64_t) x & ~0xFFF) + 0x1000)

#define sbit(map, idx) (map[(idx / 64)] |= (1 << (idx % 64)))
#define cbit(map, idx) (map[(idx / 64)] &= ~(1 << (idx % 64)))
#define rbit(map, idx) ((map[(idx / 64)] >> (idx % 64)) & 1)

typedef uint64_t* pmm_bitmap;

void pmm_init(multiboot_info_t* info);

void* pmm_malloc(uint64_t size);

uint64_t pmm_totalmem();
uint64_t pmm_totalalloc();
multiboot_info_t* pmm_mbptr();

void pmm_free(uint64_t ptr, uint64_t size);

#endif
