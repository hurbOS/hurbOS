SOURCEDIR=src

SRC=$(shell find $(SOURCEDIR) -name "*.c" -o -name "*.asm")
REAL=$(shell find $(SOURCEDIR) -name "*.real")
INCLUDE:=src/include

CC=x86_64-elf-gcc
CFLAGS= -O2                        \
	-ffreestanding                 \
	-ggdb			               \
	-I$(INCLUDE)		           \
	-fno-pic                       \
	-mcmodel=kernel		           \
	-mno-sse                       \
	-mno-sse2                      \
	-mno-mmx                       \
	-mno-80387                     \
	-mno-red-zone                  \
	-fno-stack-protector           \
	-fno-omit-frame-pointer        \
	-fno-merge-constants
OBJS=$(patsubst %.c, %.o, $(patsubst %.asm, %.o, $(SRC)))
BINS=$(patsubst %.real, %.bin, $(REAL))

EMU=qemu-system-x86_64
EFLAGS=-boot d -cdrom iso/build/out.iso -d int -s -S -m 2048M -no-reboot -monitor stdio -no-shutdown -smp 4 -machine q35

ISO=grub-mkrescue
ISOFLAGS=-o iso/build/out.iso
ISODIR=iso/src

LDFLAGS=-T linker.ld            \
        -o kernel.elf           \
        -nostdlib               \
		-no-pie                 \
		-z max-page-size=0x1000

all: $(BINS) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -lgcc

%.o: %.asm
	nasm  $^ -felf64 -gdwarf -o $@

%.o: %.c
	$(CC) -c $^ -o $@ $(CFLAGS)

%.bin: %.real
	nasm $^ -fbin -o $@

iso:
	mkdir -p iso/src/boot/grub
	mkdir -p iso/build
	cp kernel.elf iso/src/boot/kernel.elf
	$(ISO) $(ISOFLAGS) $(ISODIR)

run:
	$(EMU) $(EFLAGS)

.PHONY : clean iso run syms
clean:
	rm -f $(OBJS)
	rm -f kernel.elf
	rm -f $(BINS)

