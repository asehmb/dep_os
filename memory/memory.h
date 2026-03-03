

#ifndef DEPOS_MEMORY
#define DEPOS_MEMORY

#include <stddef.h>
#include <stdint.h>

void *kmalloc(size_t size);

void *kcalloc(size_t num, size_t size);

void kfree(void *ptr);

#endif // DEBUG
