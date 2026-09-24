

#ifndef DEPOS_MEMORY
#define DEPOS_MEMORY

#include <stddef.h>
#include <stdint.h>

extern uint64_t l0_table[];

extern uint64_t l1_table[];

extern uint64_t l2_table[];

extern uint64_t l3_table[];

void init_mem();

void *alloc_page();

void *free_page();

void *kmalloc(size_t size);

void *kcalloc(size_t num, size_t size);

void kfree(void *ptr);

#endif // DEBUG
