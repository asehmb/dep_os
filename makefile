# ========= TOOLCHAIN =========
ARCH        := aarch64
CROSS       := aarch64-elf

AS          := $(CROSS)-as
CC          := $(CROSS)-gcc
LD          := $(CROSS)-ld
OBJCOPY     := $(CROSS)-objcopy

# ========= FLAGS =========
CFLAGS  := -ffreestanding -fno-builtin -nostdlib \
           -nostartfiles -Wall -Wextra \
           -O2 -mcpu=cortex-a57

ASFLAGS := -mcpu=cortex-a57

LDFLAGS := -T linker.ld -nostdlib

# ========= FILES =========
BOOT_OBJS   := boot/bootloader.o boot/vectors.o kernel/print.o drivers/uart.o
KERNEL_OBJS := kernel/kernel.o kernel/print.o kernel/exceptions.o

OBJS := $(BOOT_OBJS) $(KERNEL_OBJS)

# ========= OUTPUT =========
KERNEL_ELF := build/kernel.elf
KERNEL_BIN := build/kernel.bin

# ========= RULES =========
all: $(KERNEL_ELF)

build:
	mkdir -p build

$(KERNEL_ELF): build $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# ========= COMPILE RULES =========
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========= UTIL =========
run: $(KERNEL_ELF)
	qemu-system-aarch64 \
	  -machine virt \
	  -cpu cortex-a57 \
	  -kernel $(KERNEL_ELF) \
	  -nographic

clean:
	rm -rf build *.o boot/*.o kernel/*.o drivers/*.o

.PHONY: all clean run
