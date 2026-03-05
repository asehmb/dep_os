

#include "../../drivers/uart.h"
#include "tcb.h"

#define TIMER_INTERVAL_TICKS 100000ULL

void rearm_timer(void) {
  uint64_t now;
  uint64_t next_tick;
  asm volatile("mrs %0, cntpct_el0" : "=r"(now));
  next_tick = now + TIMER_INTERVAL_TICKS;
  asm volatile("msr cntv_cval_el0, %0" : : "r"(next_tick));
}

void setup_timer() {
  rearm_timer();

  uint64_t timer_control_value = 1; // Enable timer, unmask interrupt
  asm volatile("msr cntv_ctl_el0, %0" : : "r"(timer_control_value));
}

void init_scheduler() {
  // Initialize the scheduler data structures
  // Write to the Timer Value register
  setup_timer();
  uart_puts("Scheduler initialized.\n");
}

void schedule(void) {
  // Save the context of the current thread
  struct task_struct *prev_thread = get_current_thread();
  save_context(prev_thread);

  // Select the next thread to run (round-robin scheduling)
  struct task_struct *next_thread = select_next_thread();

  // Load the context of the next thread
  load_context(next_thread);
}
