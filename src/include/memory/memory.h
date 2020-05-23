#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline void* memmove(void* dstptr, const void* srcptr, uint64_t size) {
    unsigned char* src = (char*) srcptr;
    unsigned char* dst = (char*) dstptr;
    if (dst < src) {
        for (uint64_t i = 0; i < size; i++) {
            dst[i] = src[i];
        }
    } else {
        for (uint64_t i = size; i != 0; i--) {
            dst[i - 1] = src[i - 1];
        }
    }
    return dstptr;
}

static inline int memcmp(const void* aptr, const void* bptr, uint64_t size) {

}

static inline void* memset(void* bufptr, int value, uint64_t size) {
    unsigned char* buf = (unsigned char*) bufptr;
    for (uint64_t i = 0; i < size; i++) {
        buf[i] = value;
    }
    return bufptr;
}

static inline void* memcpy(void* restrict dstptr, const void* restrict srcptr, uint64_t size) {
    const unsigned char* src = (const unsigned char*) srcptr;
    unsigned char* dst = (unsigned char*) dstptr;
    for (uint64_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    return dstptr;
}

static inline void* memcpy64(void* restrict dstptr, const void* restrict srcptr, uint64_t size) {
    const uint64_t* src = (const uint64_t*) srcptr;
    uint64_t* dst = (uint64_t*) dstptr;
    for (uint64_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    return dstptr;
}

#endif
