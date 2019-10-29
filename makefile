hurbOS.bin: ./build/boot.o ./build/kernel.o
	i686-elf-gcc -T linker.ld -o hurbOS.bin -ffreestanding -O2 -nostdlib ./build/kernel.o ./build/boot.o -lgcc

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc -c ./src/kernel.c -o ./build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

./build/boot.o: ./src/boot.s
	i686-elf-as ./src/boot.s -o ./build/boot.o

mkdir:
	mkdir out 
	mkdir build

clean:
	rm -R out
	rm -R build
