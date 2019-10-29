./build/boot.o: ./src/boot.s
	i686-elf-as ./src/boot.s -o ./build/boot.o

mkdir:
	mkdir out 
	mkdir build

clean:
	rm -R out
	rm -R build
