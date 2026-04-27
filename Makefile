BOOT=boot.bin
KERNEL_BIN=kernel.bin
KERNEL_ELF=kernel.elf
IMG=os-image.bin

KERNEL_SECTORS=$(shell echo $$(( ( $(shell stat -c%s kernel.bin) + 511 ) / 512 )))

all:
	nasm -f bin src/boot.asm -o $(BOOT) -DKERNEL_SECTORS=$(KERNEL_SECTORS)
	nasm -f elf32 src/kernel_entry.asm -o kernel_entry.o
	gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -c src/kernel.c -o kernel.o
	ld -m elf_i386 -Ttext 0x10000 -o $(KERNEL_ELF) kernel_entry.o kernel.o
	objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMG) bs=512 count=$$((1 + $(KERNEL_SECTORS)))
	dd if=$(BOOT) of=$(IMG) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=1 conv=notrunc

run:
	qemu-system-x86_64 -drive format=raw,file=$(IMG) -no-reboot -no-shutdown

clean:
	rm -f *.bin *.o *.elf $(IMG)