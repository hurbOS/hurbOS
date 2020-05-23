#include <mm/vmm.h>

vmm_address_space current;

vmm_address_space vmm_get_current_space() {
    return current;
}

void vmm_init() {
    asm volatile ("mov %%cr3, %0" : "=r"(current));

    vmm_address_space new = pmm_malloc(PAGE_SIZE);
    vmm_map(new, (uint64_t) new - MEM_HIGH_OFFSET, 0, current);
    memset(new, 0, PAGE_SIZE);

    if (pmm_totalmem() < VMM_4GIB) {
        for (uint64_t i = 0; i < VMM_4GIB; i+=PAGE_SIZE_2MB) {
            vmm_map_2mb(i + MEM_HIGH_OFFSET, i, 0, new);
        }
    } else {
        for (uint64_t i = 0; i < VMM_4GIB; i+=PAGE_SIZE_2MB) {
            vmm_map_2mb(i + MEM_HIGH_OFFSET, i, 0, new);
        }

        for (uint64_t i = VMM_4GIB; i < pmm_totalmem(); i+=PAGE_SIZE_2MB) {
            vmm_map_2mb(i + MEM_HIGH_OFFSET, i, 0, new);
        }
    }

    for (uint64_t i = 0; i < PAGE_SIZE_2MB * 4; i+=PAGE_SIZE_2MB) {
        vmm_map_2mb(i + KERNEL_OFFSET, i, 0, new);
    }

    vmm_change_address_space(new);
    current = new;
}

void vmm_map(uint64_t virtualaddr, uint64_t physaddr, uint64_t flags, vmm_address_space pml4) {
    uint64_t pml4index = (virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = (virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = (virtualaddr >> 21) & 0x1FF;
    uint64_t pml1index = (virtualaddr >> 12) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;
    uint64_t* pml1 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml4[pml4index] |= flags;
        pml3 = pml4[pml4index] & ADDRESS_MASK;
        pml3 = (uint64_t) pml3 + MEM_HIGH_OFFSET;
    } else {
        pml3 = pmm_malloc(PAGE_SIZE);
        pml4[pml4index] = ((uint64_t) pml3 - MEM_HIGH_OFFSET) | VMM_PRESENT | flags;
        memset(pml3, 0, PAGE_SIZE);
    }

    if (pml3[pml3index] & VMM_PRESENT) {
        pml3[pml3index] |= flags;
        pml2 = pml3[pml3index] & ADDRESS_MASK;
        pml2 = (uint64_t) pml2 + MEM_HIGH_OFFSET;
    } else {
        pml2 = pmm_malloc(PAGE_SIZE);
        pml3[pml3index] = ((uint64_t) pml2 - MEM_HIGH_OFFSET) | VMM_PRESENT | flags;
        memset(pml2, 0, PAGE_SIZE);
    }

    if (pml2[pml2index] & VMM_PRESENT) {
        pml2[pml2index] |= flags;
        pml1 = pml2[pml2index] & ADDRESS_MASK;
        pml1 = (uint64_t) pml1 + MEM_HIGH_OFFSET;
    } else {
        pml1 = pmm_malloc(PAGE_SIZE);
        pml2[pml2index] = ((uint64_t) pml1 - MEM_HIGH_OFFSET) | VMM_PRESENT | flags;
        memset(pml1, 0, PAGE_SIZE);
    }

    pml1[pml1index] = physaddr | VMM_PRESENT | flags;
}

void vmm_map_2mb(uint64_t virtualaddr, uint64_t physaddr, uint64_t flags, vmm_address_space pml4) {
    uint64_t pml4index = ((uint64_t) virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = ((uint64_t) virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = ((uint64_t) virtualaddr >> 21) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml4[pml4index] |= flags;
        pml3 = pml4[pml4index] & ADDRESS_MASK;
        pml3 = (uint64_t) pml3 + MEM_HIGH_OFFSET;
    } else {
        pml3 = pmm_malloc(PAGE_SIZE);
        pml4[pml4index] = ((uint64_t) pml3 - MEM_HIGH_OFFSET) | VMM_PRESENT | flags;
        memset(pml3, 0, PAGE_SIZE);
    }

    if (pml3[pml3index] & VMM_PRESENT) {
        pml3[pml3index] |= flags;
        pml2 = pml3[pml3index] & ADDRESS_MASK;
        pml2 = (uint64_t) pml2 + MEM_HIGH_OFFSET;
    } else {
        pml2 = pmm_malloc(PAGE_SIZE);
        pml3[pml3index] = ((uint64_t) pml2 - MEM_HIGH_OFFSET) | VMM_PRESENT | flags;
        memset(pml2, 0, PAGE_SIZE);
    }

    pml2[pml2index] = physaddr | VMM_PRESENT | VMM_LARGE | flags;
}

void vmm_chflags(uint64_t virtualaddr, vmm_address_space pml4, uint64_t flags) {
    uint64_t pml4index = (virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = (virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = (virtualaddr >> 21) & 0x1FF;
    uint64_t pml1index = (virtualaddr >> 12) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;
    uint64_t* pml1 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml3 = (pml4[pml4index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
        if (pml3[pml3index] & VMM_PRESENT) {
            pml2 = (pml3[pml3index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
            if (pml2[pml2index] & VMM_PRESENT) {
                pml1 = (pml2[pml2index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
                if (pml1[pml1index] & VMM_PRESENT) {
                    uint64_t paddr = pml1[pml1index] & ADDRESS_MASK;
                    pml1[pml1index] = paddr | flags;
                    vmm_invlpg(virtualaddr);
                }
            }
        }
    }
}

void vmm_chflags_2mb(uint64_t virtualaddr, vmm_address_space pml4, uint64_t flags) {
    uint64_t pml4index = (virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = (virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = (virtualaddr >> 21) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml3 = (pml4[pml4index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
        if (pml3[pml3index] & VMM_PRESENT) {
            pml2 = (pml3[pml3index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
            if (pml2[pml2index] & VMM_PRESENT) {
                uint64_t paddr = pml2[pml2index] & ADDRESS_MASK;
                pml2[pml2index] = paddr | flags | VMM_LARGE;
                vmm_invlpg(virtualaddr);
            }
        }
    }
}

void vmm_unmap(uint64_t virtualaddr, vmm_address_space pml4) {
    uint64_t pml4index = (virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = (virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = (virtualaddr >> 21) & 0x1FF;
    uint64_t pml1index = (virtualaddr >> 12) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;
    uint64_t* pml1 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml3 = (pml4[pml4index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
        if (pml3[pml3index] & VMM_PRESENT) {
            pml2 = (pml3[pml3index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
            if (pml2[pml2index] & VMM_PRESENT) {
                pml1 = (pml2[pml2index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
                if (pml1[pml1index] & VMM_PRESENT) {
                    pml1[pml1index] = 0;
                }
            }
        }
    }
}

void vmm_unmap_2mb(uint64_t virtualaddr, vmm_address_space pml4) {
    uint64_t pml4index = (virtualaddr >> 39) & 0x1FF;
    uint64_t pml3index = (virtualaddr >> 30) & 0x1FF;
    uint64_t pml2index = (virtualaddr >> 21) & 0x1FF;

    uint64_t* pml3 = NULL;
    uint64_t* pml2 = NULL;

    if (pml4[pml4index] & VMM_PRESENT) {
        pml3 = (pml4[pml4index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
        if (pml3[pml3index] & VMM_PRESENT) {
            pml2 = (pml3[pml3index] & ADDRESS_MASK) + MEM_HIGH_OFFSET;
            if (pml2[pml2index] & VMM_PRESENT) {
                pml2[pml2index] = 0;
            }
        }
    }
}

vmm_address_space vmm_new_address_space() {
    vmm_address_space new = pmm_malloc(PAGE_SIZE);
    memset(new, 0, PAGE_SIZE);
    for(uint64_t i = (ENTRIES_PER_PML / 2) - 1; i < ENTRIES_PER_PML; i++) {
        new[i] = current[i];
    }
    return new;
}

uint64_t pmls_to_virt(uint64_t pml4index, uint64_t pml3index, uint64_t pml2index, uint64_t pml1index) {
    uint64_t ret = 0;
    ret |= pml4index << 39;
    ret |= pml3index << 30;
    ret |= pml2index << 21;
    ret |= pml1index << 12;
    return ret;
}
 
vmm_address_space vmm_fork_address_space(vmm_address_space source) {
    vmm_address_space space = vmm_new_address_space();
    for (uint64_t i = 0; i < ENTRIES_PER_PML / 2; i++) {
        if (source[i] & VMM_PRESENT) {
            uint64_t* pml3 = source[i] & ADDRESS_MASK;
            if (pml3 < MEM_HIGH_OFFSET) {
                pml3 = (uint64_t) pml3 + MEM_HIGH_OFFSET;
            }
            for (uint64_t j = 0; j < ENTRIES_PER_PML; j++) {
                if (pml3[j] & VMM_PRESENT) {
                    uint64_t* pml2 = pml3[j] & ADDRESS_MASK;
                    if (pml2 < MEM_HIGH_OFFSET) {
                        pml2 = (uint64_t) pml2 + MEM_HIGH_OFFSET;
                    }

                    for (uint64_t k = 0; k < ENTRIES_PER_PML; k++) {
                        if (pml2[k] & VMM_PRESENT) {
                            uint64_t* pml1 = pml2[k] & ADDRESS_MASK;

                            if (pml2[k] & VMM_LARGE) {
                                uint64_t dstpg = (uint64_t) pmm_malloc(PAGE_SIZE_2MB) - MEM_HIGH_OFFSET;
                                uint64_t srcpg = pml1;
                                if (srcpg < MEM_HIGH_OFFSET) {
                                    srcpg = srcpg + MEM_HIGH_OFFSET;
                                }
                                
                                uint64_t flags = pml2[k] & (~ADDRESS_MASK);
                            
                                vmm_map_2mb(pmls_to_virt(i, j, k, 0), dstpg, flags, space);
                            
                                memcpy64(dstpg, srcpg, PAGE_SIZE_2MB / sizeof(uint64_t));
                            } else {
                                for (uint64_t l = 0; l < ENTRIES_PER_PML; l++) {
                                    if (pml1[l] & VMM_PRESENT) {
                                        uint64_t dstpg = (uint64_t) pmm_malloc(PAGE_SIZE) - MEM_HIGH_OFFSET;
                                        uint64_t srcpg = pml1[l] & ADDRESS_MASK;
                                        if (srcpg < MEM_HIGH_OFFSET) {
                                            srcpg = srcpg + MEM_HIGH_OFFSET;
                                        }
                            
                                        uint64_t flags = pml1[l] & (~ADDRESS_MASK);
                                        vmm_map(pmls_to_virt(i, j, k, l), dstpg, flags, space);
                            
                                        memcpy64(dstpg, srcpg, PAGE_SIZE / sizeof(uint64_t));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return space;
}

void vmm_delete_address_space(vmm_address_space space) {
    for (uint64_t i = 0; i < ENTRIES_PER_PML / 2; i++) {
        if (space[i] & VMM_PRESENT) {
            uint64_t* pml3 = space[i] & ADDRESS_MASK;
            if (pml3 < MEM_HIGH_OFFSET) {
                pml3 = (uint64_t) pml3 + MEM_HIGH_OFFSET;
            }
            for (uint64_t j = 0; j < ENTRIES_PER_PML; j++) {
                if (pml3[j] & VMM_PRESENT) {
                    uint64_t* pml2 = pml3[j] & ADDRESS_MASK;
                    if (pml2 < MEM_HIGH_OFFSET) {
                        pml2 = (uint64_t) pml2 + MEM_HIGH_OFFSET;
                    }
                    if ((uint64_t) pml2 & VMM_LARGE) {
                        pmm_free(pml2, PAGE_SIZE_2MB);
                    } else {
                        for (uint64_t k = 0; k < ENTRIES_PER_PML; k++) {
                            if (pml2[k] & VMM_PRESENT) {
                                uint64_t* pml1 = pml2[k] & ADDRESS_MASK;
                                if (pml1 < MEM_HIGH_OFFSET) {
                                    pml1 = (uint64_t) pml1 + MEM_HIGH_OFFSET;
                                }
                                for (uint64_t l = 0; l < ENTRIES_PER_PML; l++) {
                                    if (pml1[l] & ADDRESS_MASK >= MEM_HIGH_OFFSET) {
                                        pmm_free(pml1[l] & ADDRESS_MASK, PAGE_SIZE);
                                    } else {
                                        pmm_free(((uint64_t) pml1[l] & ADDRESS_MASK + MEM_HIGH_OFFSET), PAGE_SIZE);
                                    }
                                }
                                pmm_free(pml1, PAGE_SIZE);
                            }
                        }
                        pmm_free(pml2, PAGE_SIZE);
                    }
                }
            }
            pmm_free(pml3, PAGE_SIZE);
        }
    }

    pmm_free(space, PAGE_SIZE);
}

void vmm_change_address_space(vmm_address_space space) {
    asm volatile("mov %0, %%cr3; \
                  mov %%cr3, %%rax; \
                  mov %%rax, %%cr3"
                 :
                 : "r"(((uint64_t) space) - MEM_HIGH_OFFSET));
}
