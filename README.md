# dep_os

`dep_os` is an experimental, educational AArch64 kernel that runs on QEMU's `virt` machine.

It currently focuses on boot, exception handling, UART output, timer interrupts, and scheduler scaffolding.

## What works

- AArch64 EL1 entry (`_start`) with vector table setup (`VBAR_EL1`).
- UART PL011-style character I/O (`drivers/uart.c`).
- Exception vector + context save/restore path (`boot/vectors.s`).
- Sync/IRQ/FIQ/SError C handlers with register dump and panic path.
- Basic syscall dispatch for:
  - `SYSCALL_WRITE`
  - `SYSCALL_EXIT`
- Timer interrupt re-arming via virtual timer (`cntv_*`) and GICv2 CPU interface setup.
- Basic scheduler/task-control-block structures and context save/load helpers.

## WIP (work in progress)

- Full scheduler integration:
  - Timer is firing and re-arming.
  - `schedule()` exists but is not yet wired into the IRQ/timer path.
- Process model maturity:
  - User-mode flow is partial (`g_user_return_elr`, user stack scaffolding).
- Syscalls:
  - Syscall surface is minimal and not POSIX-complete at the moment.
- Memory management:
  - Simple bump-style `kmalloc`.
  - `kcalloc`/`kfree` are declared but not implemented.

## Repository layout

- `boot/` - early boot and exception vector assembly.
- `kernel/` - kernel entry, exception handling, scheduler code.
- `drivers/` - UART driver.
- `memory/` - memory allocator interfaces.
- `linker.ld` - linker script and memory layout.
- `makefile` - cross-build + run targets.
- `run.sh` - direct QEMU run helper.

## Build

Requirements:

- AArch64 cross toolchain (`aarch64-elf-gcc`, `aarch64-elf-as`, `aarch64-elf-ld`, `aarch64-elf-objcopy`)
- `qemu-system-aarch64`

Build kernel ELF:

```bash
make
```

Output:

- `build/kernel.elf`

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
