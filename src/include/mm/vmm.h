#ifndef VMM
#define VMM

#include <stddef.h>
#include <stdint.h>
#include <multiboot/multiboot.h>
#include <mm/pmm.h>
#include <memory/memory.h>

#define VMM_PRESENT (1 << 0)
#define VMM_WRITE (1 << 1)
#define VMM_USER (1 << 2)
#define VMM_WT (1 << 3)
#define VMM_NCACHE (1 << 4)
#define VMM_DIRTY (1 << 5)
#define VMM_LARGE (1 << 7)
#define VMM_PAGEOUT (1 << 9)
#define VMM_ALLOCATED (1 << 10)
#define VMM_CONTINUE (1 << 11)
#define VMM_NX (1 << 63)

#define VMM_4GIB (4294967296)

#define ADDRESS_MASK ~0xFFF
#define ALIGN4K(x) ((x & ADDRESS_MASK) + 0x1000)

#define ENTRIES_PER_PML 512

#define MEM_HIGH_OFFSET 0xFFFF800000000000
#define KERNEL_OFFSET 0xFFFFFFFF80000000

#define vmm_invlpg(x) asm volatile ( "invlpg (%0)" : : "b"(x) : "memory")

void vmm_init();

typedef uint64_t* vmm_address_space;

void vmm_map(uint64_t virtualaddr, uint64_t physaddr, uint64_t flags, vmm_address_space pml4);
void vmm_unmap(uint64_t virtualaddr, vmm_address_space pml4);

void vmm_map_2mb(uint64_t virtualaddr, uint64_t physaddr, uint64_t flags, vmm_address_space pml4);
void vmm_unmap(uint64_t virtualaddr, vmm_address_space pml4);

void vmm_chflags(uint64_t virtualaddr, vmm_address_space pml4, uint64_t flags);
void vmm_chflags_2mb(uint64_t virtualaddr, vmm_address_space pml4, uint64_t flags);

vmm_address_space vmm_new_address_space();
vmm_address_space vmm_fork_address_space(vmm_address_space source);
vmm_address_space vmm_get_current_space();
void vmm_delete_address_space(vmm_address_space space);
void vmm_change_address_space(vmm_address_space space);

#endif
