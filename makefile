

bootloader:
	nasm -f bin bootloader.s -o bootloader.bin

.PHONY: clean

clean:
	rm -f bootloader.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.bin
	rm -f *.img
	rm -f *.iso
	rm -f *.map
	rm -f *.sym
	rm -rf build/