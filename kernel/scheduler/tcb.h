
#ifndef DEPOS_TCB_H
#define DEPOS_TCB_H

#include <stdint.h>

struct cpu_context {
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t fp; // x29: Frame pointer
  uint64_t lr; // x30: Link register
  uint64_t sp; // Stack pointer
};

struct task_struct {
  struct cpu_context cpu_context; // Must be at offset 0 for assembly access!
  uint64_t state;                 // e.g., RUNNING, WAITING, DEAD
  uint64_t priority;
  // ... other info like Process ID (PID)
};

#define MAX_TASKS 4
extern void cpu_switch_to(struct task_struct *prev,
                          struct task_struct *next); // define in cpu_switch.s

void save_context(struct task_struct *task);

void load_context(struct task_struct *task);

struct task_struct *select_next_thread();

struct task_struct *get_current_thread();

#endif // DEPOS_TCB_H
