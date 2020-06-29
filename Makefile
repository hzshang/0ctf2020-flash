#
# Makefile
# hzshang, 2020-06-17 18:32
#
.SUFFIXES:
.PRECIOUS:

PREFIX=mips-linux-gnu-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
OBJCOPY=$(PREFIX)objcopy
RM=rm -f
QEMU=qemu-system-mips

CFLAGS=-fno-stack-protector -nostdlib -mno-abicalls -fno-pic -O0 -g -Isrc/include# -DDEBUG
BIOS_SRC=$(wildcard src/bios/*.c)
BIOS_OBJ=$(BIOS_SRC:.c=.o) src/bios/bios_start.So

KERNEL_SRC=$(filter-out src/kernel/intr.c,$(wildcard src/kernel/*.c))
KERNEL_OBJ=$(KERNEL_SRC:.c=_new.o) $(KERNEL_SRC:.c=_obf.o) src/kernel/intr.o src/kernel/kernel_start.So src/kernel/vm.So
# KERNEL_OBJ=$(KERNEL_SRC:.c=.o) src/kernel/intr.o src/kernel/kernel_start.So src/kernel/vm.So


.PHONY: clean run debug

all: flash
	cp flash flash_server
	echo -n flag{0ff_by_one_crashed_machine_fr0m_kernel_t0_bi0s} | dd of=flash_server conv=notrunc bs=1 seek=$$(cat flash_server | grep -aob 'flag{the_second_flag_is_here}' | grep -oE '[0-9]+')
	mkdir -p deploy/player
	mkdir -p deploy/server
	cp flash deploy/player/flash
	cp flash_server deploy/server/flash
	gcc -static tool/wrapper.c -o deploy/server/wrapper

run: flash
	$(QEMU) -M mips -bios flash -nographic

debug: flash
	$(QEMU) -M mips -bios flash -nographic -s -S

flash: bios.elf kernel.elf
	$(OBJCOPY) -S -O binary -j .text bios.elf bios.bin
	$(OBJCOPY) -S -O binary -j .text kernel.elf kernel.bin
	$(OBJCOPY) -S -O binary -j .rodata kernel.elf obf.bin
	python3 trans_enc.py ./obf.bin ./obf.bin.enc
	dd if=/dev/urandom of=$@ bs=512 count=400
	dd if=bios.bin of=$@ conv=notrunc
	dd if=kernel.bin of=$@ seek=65536 bs=1 conv=notrunc
	dd if=obf.bin.enc of=$@ seek=131072 bs=1 conv=notrunc
	$(RM) obf.bin.enc

bios.elf: $(BIOS_OBJ)
	$(LD) -T src/bios/bios.ld $^ -o $@

kernel.elf: $(KERNEL_OBJ)
	$(LD) -T src/kernel/kernel.ld $^ -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $^ -o $@

src/kernel/%_new.s src/kernel/%_obf.s: src/kernel/%.s
	python3 ./obf.py $^

%.s: %.c
	$(CC) $(CFLAGS) -S $^ -o $@

src/kernel/vm.S: do_asm.py
	python3 $^ $@

%.So: %.S
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(RM) src/*/*.s src/*/*.o src/*/*.So bios.elf kernel.elf bios.bin kernel.bin obf.bin flash flash_server deploy/player/flash deploy/server/flash deploy/server/wrapper
