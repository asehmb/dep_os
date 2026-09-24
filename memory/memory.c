

#include "memory.h"
#include "../drivers/uart.h"
#include <stdint.h>
#define BLOCK_DESCIPTOR 0b01

#define CREATE_TABLE_DESCRIPTOR(next_table_phys_addr)                          \
  (((uint64_t)(next_table_phys_addr) & 0x0000FFFFFFFFF000ULL) |                \
   0x3) // Bit[1:0] = 11 (Table)

#define CREATE_L2_BLOCK_DESCRIPTOR(phys_addr, mair_idx)                        \
  (((uint64_t)(phys_addr) & 0x0000FFFFFFE00000ULL) |                           \
   (1UL << 10) |                 /* Access Flag (AF) = 1 */                    \
   ((uint64_t)(mair_idx) << 2) | /* AttrIndx from MAIR_EL1 */                  \
   (3UL << 8) |                  /* Inner Shareable */                         \
   0x1)                          /* Bit[1:0] = 01 (Block) */

// clangd error, ignore
__attribute__((section(".page_tables"),
               aligned(4096))) uint64_t l2_ram_table[512];

void setup_tables() {
  // 0. Clear tables
  for (int i = 0; i < 512; i++) {
    l0_table[i] = 0;
    l1_table[i] = 0;
    l2_table[i] = 0;
    l2_ram_table[i] = 0;
  }

  // 1. L0 Entry 0 (0-512GB) points to L1
  l0_table[0] = CREATE_TABLE_DESCRIPTOR(l1_table);

  // 2. L1 Entry 0 (0-1GB) points to L2 MMIO Table
  l1_table[0] = CREATE_TABLE_DESCRIPTOR(l2_table);

  // 3. L1 Entry 1 (1-2GB) points to L2 RAM Table
  l1_table[1] = CREATE_TABLE_DESCRIPTOR(l2_ram_table);

  // --- Populate MMIO (0GB - 1GB) in l2_table ---
  // GIC at 0x08000000 -> Index 64
  l2_table[64] = CREATE_L2_BLOCK_DESCRIPTOR(0x08000000, 0); // MAIR 0 = Device
  // UART at 0x09000000 -> Index 72
  l2_table[72] = CREATE_L2_BLOCK_DESCRIPTOR(0x09000000, 0); // MAIR 0 = Device

  // --- Populate RAM (1GB - 2GB) in l2_ram_table ---
  // Map 64MB RAM (Indices 0 to 31) starting at Physical Address 0x40000000
  for (uint64_t i = 0; i < 32; i++) {
    uint64_t phys = 0x40000000 + (i * 0x200000); // 2MB steps starting at 1GB
    l2_ram_table[i] =
        CREATE_L2_BLOCK_DESCRIPTOR(phys, 3); // MAIR 3 = Normal Cacheable
  }
}

void enable_mmu() {
  // pass l0 table to TTBR0_EL1
  asm volatile("msr TTBR0_EL1, %0" ::"r"(l0_table));
  asm volatile("dsb ish \n\t"
               "tlbi vmalle1is \n\t"
               "dsb ish \n\t"
               "isb \n\t");

  // 3. Read SCTLR_EL1, enable M (bit 0), C (bit 2), and I (bit 12)
  uint64_t sctlr;
  asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
  sctlr |= (1UL << 0) | // M: Enable MMU
           (1UL << 2) | // C: Enable Data Cache
           (1UL << 12); // I: Enable Instruction Cache
  asm volatile("msr sctlr_el1, %0" ::"r"(sctlr));

  // 4. Instruction Synchronization Barrier to synchronize context switch
  asm volatile("isb");
}

void test_mmu_ram(void) {
  volatile uint64_t *ptr =
      (volatile uint64_t *)0x40100000; // Inside mapped RAM space
  *ptr = 0xDEADBEEFCAFECAFEULL;

  if (*ptr == 0xDEADBEEFCAFECAFEULL) {
    uart_puts("RAM Read/Write Test: PASSED\n");
  } else {
    uart_puts("RAM Read/Write Test: FAILED\n");
  }
}

void init_mem() {

  uint64_t el_level;
  asm volatile("mrs %0, CurrentEL" : "=r"(el_level));
  uart_puts("CurrentEL: ");
  uart_putc(((el_level >> 2) & 0x3) + '0');
  uart_putc('\n');

  // setup mair_el1
  asm volatile("LDR X0, =0x00000000FF440400");
  asm volatile("MSR mair_el1, x0");

  // Setup TCR_EL1: 4KB granule, 48-bit VA regions for both TTBR0 & TTBR1,
  // 48-bit Physical Address limit, Inner Shareable, Cacheable walks.
  asm volatile("LDR      X0, =0x5B5103510");
  asm volatile("MSR      TCR_EL1, X0");

  setup_tables();

  enable_mmu();
  uart_puts("MMU enabled!\n");

  test_mmu_ram();

  return;
}

void *alloc_page() {
  uart_puts("alloc_page");

  uintptr_t sp_val;
  // Reads the sp register and moves it into the sp_val variable
  __asm__ volatile("mov %0, sp" : "=r"(sp_val));
  uart_puts((char *)sp_val);
  return NULL;
}
