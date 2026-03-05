

#include "../../drivers/uart.h"
#include "tcb.h"

void setup_timer() {

  // Assuming frequency is 10MHz, 10ms = 100,000 ticks
  // If frequency is 1MHz, 10ms = 10,000 ticks
  uint64_t timer_ticks_for_10ms =
      100000; // Adjust this value based on the timer frequency
  asm volatile("msr cntp_tval_el0, %0" : : "r"(timer_ticks_for_10ms));

  // Enable the timer
  uint64_t timer_control_value = 1; // Enable the timer (bit 0)
  asm volatile("msr cntp_ctl_el0, %0" : : "r"(timer_control_value));
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
