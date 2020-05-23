#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#define PREFIX(func) k##func

uint64_t liballoc_lock();
uint64_t liballoc_unlock();

void* liballoc_alloc(uint64_t);

uint64_t liballoc_free(void*,uint64_t);

void *PREFIX(malloc)(uint64_t);
void *PREFIX(realloc)(void *, uint64_t);
void *PREFIX(calloc)(uint64_t, uint64_t);
void  PREFIX(free)(void *);

#endif
