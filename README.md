# dep_os

`dep_os` is an experimental, educational AArch64 kernel that runs on QEMU's `virt` machine.

It currently focuses on boot, exception handling, virtual memory, UART output, timer interrupts, and scheduler scaffolding.

## What works

- **AArch64 Bootloader:** Higher-half kernel boot with temporary page tables and EL1 entry point.
- **Virtual Memory:** Identity-mapped lower half with higher-half kernel mapping using 4-level page tables (`L0-L3`).
- **UART I/O:** PL011-style character I/O via `drivers/uart.c` with `kprintf` support.
- **Exception Handling:**
  - Full exception vector table (`boot/vectors.s`) with context save/restore.
  - Sync/IRQ/FIQ/SError handlers in `kernel/exceptions.c` with complete register dumps.
  - Page fault decoding and kernel panic path.
- **Syscall Dispatch:**
  - `SYSCALL_WRITE` - write to stdout (fd=1).
  - `SYSCALL_EXIT` - process exit.
  - `SYSCALL_GETPID` - placeholder for future implementation.
  - SVC instruction handling with ELR/SPSR manipulation for user-space returns.
- **Timer & Interrupts:**
  - ARM Generic Timer (virtual timer via `cntv_*` registers).
  - GICv2 CPU interface initialization and interrupt acknowledgment.
  - Timer re-arming at configurable intervals (default: 100,000 ticks).
- **Scheduler Infrastructure:**
  - Task Control Block (TCB) structures in `kernel/scheduler/tcb.c`.
  - Context save/load helpers in `kernel/scheduler/cpu_switch.s`.
  - `schedule()` function framework with thread selection logic.
  - `init_scheduler()` that sets up the timer and boots the scheduler.
- **Memory Management:**
  - Basic `kmalloc()` implementation.
  - Page allocation framework via `alloc_page()`.
  - `kcalloc()`/`kfree()` declarations (not yet implemented).
- **Kernel Library:**
  - `kprintf()` with format specifiers (`%d`, `%c`, `%s`) for kernel logging.

## WIP (work in progress)

- **Full Scheduler Integration:**
  - Timer fires and re-arms correctly.
  - `schedule()` exists but is not yet wired into the IRQ/timer handler path.
  - No task queue or context switching currently triggered by timer interrupts.
- **User-Space Execution:**
  - Partial user-mode flow via syscalls (`SVC` instruction).
  - User stack scaffolding present but not fully integrated.
  - Return from user-space to kernel via `g_user_return_elr` global.
- **Memory Management Maturity:**
  - `kmalloc()` is bump-style, not a full allocator.
  - `kcalloc()`/`kfree()` not implemented.
  - No PMM (Physical Memory Manager) for free page tracking.
- **Hardware Support:**
  - GIC Distributor initialization incomplete (only CPU interface configured).
  - Timer fine-tuning for realistic multitasking intervals.

## Repository layout

- `boot/` - bootloader, exception vectors, and EL1 entry (`bootloader.s`, `vectors.s`).
- `kernel/` - kernel core, exception handling, and klib.
  - `kernel.c` - kernel main entry.
  - `exceptions.c` - exception handlers and syscall dispatch.
  - `syscall.h` - syscall number definitions.
  - `scheduler/` - scheduler, TCB, and context switching.
  - `klib/` - kernel utilities (kprintf).
- `drivers/` - hardware drivers (UART).
- `memory/` - memory allocator interfaces and page table setup.
- `linker.ld` - linker script with memory layout and sections.
- `makefile` - cross-compilation targets and build configuration.
- `run.sh` - QEMU launch helper.
- `roadmap.md` - detailed development roadmap (Phase 1-12).

## Build

Requirements:

- AArch64 cross toolchain (`aarch64-elf-gcc`, `aarch64-elf-as`, `aarch64-elf-ld`, `aarch64-elf-objcopy`)
- `qemu-system-aarch64`

Build kernel ELF:

```bash
make
```

Output:

- `build/kernel.elf` - ELF kernel image

Clean build artifacts:

```bash
make clean
```

## Run

Using make target:

```bash
make run
```

Or helper script:

```bash
./run.sh
```

QEMU will boot the kernel in EL1 mode on a virtual AArch64 machine. You should see kernel boot messages via UART output.

## Project Status

The kernel is in **Phase 3-4 transition** (see `roadmap.md`):
- ✅ Phase 1: Basic memory layout and higher-half boot complete.
- ✅ Phase 2: Exception handling and debugging infrastructure in place.
- ✅ Phase 3: Hardware interrupts (GICv2) and timer basic setup.
- 🚧 Phase 4: Timer-driven scheduling (scaffolding complete, wiring in progress).
- ❌ Phase 5+: User-space, filesystems, and userland (future work).

## Code Statistics

- ~807 lines of C and assembly across core kernel subsystems.
- Minimal, focused codebase suitable for educational exploration.

---

*This README was generated with AI assistance to reflect the current state of the project.*
