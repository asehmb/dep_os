#include "memory.h"
#include "../kernel/klib/kprintf.h"
#include <stdint.h>

#define KERNEL_VIRT_BASE 0xFFFF800000000000ULL
#define VIRT_TO_PHYS(addr) ((uint64_t)(addr) - KERNEL_VIRT_BASE)

#define BLOCK_DESCRIPTOR 0b01

#define CREATE_TABLE_DESCRIPTOR(next_table_virt_addr)                          \
  (((VIRT_TO_PHYS(next_table_virt_addr)) & 0x0000FFFFFFFFF000ULL) |            \
   0x3) // Bit[1:0] = 11 (Table)

#define CREATE_L2_BLOCK_DESCRIPTOR(phys_addr, mair_idx)                        \
  (((uint64_t)(phys_addr) & 0x0000FFFFFFE00000ULL) |                           \
   (1UL << 10) |                 /* Access Flag (AF) = 1 */                    \
   ((uint64_t)(mair_idx) << 2) | /* AttrIndx from MAIR_EL1 */                  \
   (3UL << 8) |                  /* Inner Shareable */                         \
   0x1)                          /* Bit[1:0] = 01 (Block) */

// Helper macro to swap endianness
#define FDT_SWAP32(val) __builtin_bswap32(val)
#define FDT_SWAP64(val) __builtin_bswap64(val)

__attribute__((section(".page_tables"),
               aligned(4096))) uint64_t l2_ram_table[512];

extern uint64_t l0_table[512];
extern uint64_t l1_table[512];
extern uint64_t l2_table[512];
extern uintptr_t _kernel_end;

struct fdt_header {
  uint32_t magic;             // Magic number: 0xd00dfeed
  uint32_t totalsize;         // Total size in bytes
  uint32_t off_dt_struct;     // Offset to structure block
  uint32_t off_dt_strings;    // Offset to strings block
  uint32_t off_mem_rsvmap;    // Offset to memory reservation block
  uint32_t version;           // Format version
  uint32_t last_comp_version; // Last compatible version
  uint32_t boot_cpuid_phys;   // Which physical CPU id is booting
  uint32_t size_dt_strings;   // Size of the strings block
  uint32_t size_dt_struct;    // Size of the structure block
};

void check_dtb(void *dtb_ptr) {
  struct fdt_header *header = (struct fdt_header *)dtb_ptr;

  // Remember to swap the bytes!
  uint32_t magic = FDT_SWAP32(header->magic);

  if (magic == 0xd00dfeed) {
    kprintf("Valid DTB found!\n");
  } else {
    kprintf("Invalid DTB magic: %x\n", magic);
  }
}

void setup_tables() {
  for (int i = 0; i < 512; i++) {
    l0_table[i] = 0;
    l1_table[i] = 0;
    l2_table[i] = 0;
    l2_ram_table[i] = 0;
  }

  //  L0 Entry 256 (0xFFFF800000000000) points to L1
  l0_table[256] = CREATE_TABLE_DESCRIPTOR(l1_table);

  // L1 Entry 0 (0-1GB offset) points to L2 MMIO Table
  l1_table[0] = CREATE_TABLE_DESCRIPTOR(l2_table);

  // L1 Entry 1 (1-2GB offset) points to L2 RAM Table
  l1_table[1] = CREATE_TABLE_DESCRIPTOR(l2_ram_table);

  // Populate MMIO (0GB - 1GB) in l2_table
  l2_table[64] =
      CREATE_L2_BLOCK_DESCRIPTOR(0x08000000, 0); // GIC (MAIR 0 = Device)
  l2_table[72] =
      CREATE_L2_BLOCK_DESCRIPTOR(0x09000000, 0); // UART (MAIR 0 = Device)

  // Populate RAM (1GB - 2GB) in l2_ram_table
  for (uint64_t i = 0; i < 32; i++) {
    uint64_t phys =
        0x40000000 + (i * 0x200000); // 2MB steps starting at 1GB phys
    l2_ram_table[i] =
        CREATE_L2_BLOCK_DESCRIPTOR(phys, 3); // MAIR 3 = Normal Cacheable
  }
}

void test_mmu_ram(void) {
  // Virtual address inside high-half mapped RAM space
  volatile uint64_t *ptr =
      (volatile uint64_t *)(KERNEL_VIRT_BASE + 0x40100000ULL);
  *ptr = 0xDEADBEEFCAFECAFEULL;

  if (*ptr == 0xDEADBEEFCAFECAFEULL) {
    kprintf("RAM Read/Write Test: PASSED\n");
  } else {
    kprintf("RAM Read/Write Test: FAILED\n");
  }
}

void init_mem() {
  uint64_t el_level;
  asm volatile("mrs %0, CurrentEL" : "=r"(el_level));
  kprintf("CurrentEL: %d\n", ((el_level >> 2) & 0x3));

  setup_tables();

  // Pass PHYSICAL address of l0_table to TTBR1_EL1
  uint64_t l0_phys = VIRT_TO_PHYS(l0_table);
  asm volatile("msr TTBR1_EL1, %0" ::"r"(l0_phys));

  kprintf("Fine-grained C page tables active in TTBR1_EL1!\n");
  test_mmu_ram();

  // dtb stuff in lower half still use it for now as its only a couple bytes
  // shouldn't be an issue i think
  kprintf("DTB: %x\n", *dtb_ptr);
  check_dtb(dtb_ptr);

  // Tear down lower-half identity mapping in TTBR0_EL1
  asm volatile("msr TTBR0_EL1, %0" ::"r"(0ULL));

  // Invalidate TLB so lower-half mappings are purged immediately
  asm volatile("dsb ish \n\t"
               "tlbi vmalle1is \n\t"
               "dsb ish \n\t"
               "isb \n\t");
}

// Basic 4KB Page Frame Allocator (Bump Allocator)
static uintptr_t next_free_page = 0;

void *alloc_page() {
  if (next_free_page == 0) {
    // Align starting pointer to the next 4KB boundary past the kernel end
    next_free_page = (((uintptr_t)&_kernel_end) + 0xFFF) & ~0xFFF;
  }

  // Ensure we don't allocate past the 64MB mapped RAM ceiling (0x44000000 phys)
  if (next_free_page >= (KERNEL_VIRT_BASE + 0x44000000ULL)) {
    kprintf("Error: Out of physical memory!\n");
    return NULL;
  }

  void *page = (void *)next_free_page;
  next_free_page += 4096;

  // Zero out allocated page
  uint64_t *ptr = (uint64_t *)page;
  for (int i = 0; i < 512; i++) {
    ptr[i] = 0;
  }

  return page;
}
