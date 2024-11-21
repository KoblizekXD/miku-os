CC := gcc
AR := ar
LD := ld

ARCH := x86_64

BUILD   := build
SRC     := src
INCLUDE := include
OUTPUT  := kernel.bin
IMAGE_NAME := $(BUILD)/$(OUTPUT)

CFLAGS := \
	-g -O2 -pipe \
	-Wall \
	-Wextra \
	-std=gnu11 \
	-nostdinc \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-PIC \
	-ffunction-sections \
	-fdata-sections \
	-m64 \
	-march=x86-64 \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel
NASMFLAGS := -F dwarf -g -Wall -f elf64
LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -gc-sections -T linker.ld -m elf_x86_64


CFILES    := $(shell cd $(SRC) && find -L * -type f -name '*.c' | LC_ALL=C sort)
NASMFILES := $(shell cd $(SRC) && find -L * -type f -name '*.asm' | LC_ALL=C sort)
OBJ       := $(addprefix $(BUILD)/,$(CFILES:.c=.c.o) $(NASMFILES:.asm=.asm.o))
CPPFLAGS  := -I $(SRC) -I $(INCLUDE) \
    -isystem freestnd-c-hdrs-0bsd \
    -MMD \
    -MP

HEADER_DEPS := $(addprefix $(BUILD)/,$(CFILES:.c=.c.d))

.PHONY: all all-hdd run run-hdd kernel clean distclean run-x86_64

all: $(IMAGE_NAME).iso

all-hdd: $(IMAGE_NAME).hdd

run: run-$(ARCH)

run-hdd: run-hdd-$(ARCH)

run-debug: ovmf/ovmf-code-$(ARCH).fd $(IMAGE_NAME).iso
	qemu-system-$(ARCH) \
		-s \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom $(IMAGE_NAME).iso \
		-m 2G

run-x86_64: ovmf/ovmf-code-$(ARCH).fd $(IMAGE_NAME).iso
	qemu-system-$(ARCH) \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom $(IMAGE_NAME).iso \
		-m 2G

run-hdd-x86_64: ovmf/ovmf-code-$(ARCH).fd $(IMAGE_NAME).hdd
	qemu-system-$(ARCH) \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-hda $(IMAGE_NAME).hdd \
		-m 2G

ovmf/ovmf-code-$(ARCH).fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-$(ARCH).fd

limine/limine:
	rm -rf limine
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
	$(MAKE) -C limine

kernel-deps:
	./get-deps.sh
	touch kernel-deps

kernel: kernel-deps $(BUILD)/$(OUTPUT)

$(IMAGE_NAME).iso: limine/limine kernel
	rm -rf $(BUILD)/iso_root
	mkdir -p $(BUILD)/iso_root/boot
	cp -v $(IMAGE_NAME) $(BUILD)/iso_root/boot/
	mkdir -p $(BUILD)/iso_root/boot/limine
	cp -v limine.conf $(BUILD)/iso_root/boot/limine/
	mkdir -p $(BUILD)/iso_root/EFI/BOOT
	cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin $(BUILD)/iso_root/boot/limine/
	cp -v limine/BOOTX64.EFI $(BUILD)/iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI $(BUILD)/iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(BUILD)/iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso

$(IMAGE_NAME).hdd: limine/limine kernel
	rm -f $(IMAGE_NAME).hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).hdd
	sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00
	./limine/limine bios-install $(IMAGE_NAME).hdd
	mformat -i $(IMAGE_NAME).hdd@@1M
	mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M $(BUILD)/kernel ::/boot
	mcopy -i $(IMAGE_NAME).hdd@@1M limine.conf ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

$(BUILD)/cc-runtime/cc-runtime.a: $(BUILD)/cc-runtime-old/*
	rm -rf $(BUILD)/cc-runtime
	cp -r $(BUILD)/cc-runtime-old $(BUILD)/cc-runtime
	$(MAKE) -C $(BUILD)/cc-runtime -f cc-runtime.mk \
		CC="$(CC)" \
		AR="$(AR)" \
		CFLAGS="$(CFLAGS)" \
		CPPFLAGS='-isystem ../../freestnd-c-hdrs-0bsd -DCC_RUNTIME_NO_FLOAT'

$(BUILD)/$(OUTPUT): linker.ld $(OBJ) $(BUILD)/cc-runtime/cc-runtime.a res/font.o
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) $(BUILD)/cc-runtime/cc-runtime.a res/font.o $(LDFLAGS) -o $@

$(BUILD)/%.c.o: $(SRC)/%.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.asm.o: src/%.asm
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

res/font.o: res/font.psfu
	cd res && objcopy -O elf64-x86-64 -B i386 -I binary font.psfu font.o

clean:
	$(MAKE) -f Makefile.kernel clean

distclean:
	$(MAKE) -f Makefile.kernel distclean
