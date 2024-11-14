all:
		gcc hello_world.c -o normal
		gcc -no-pie hello_world.c -o nopie
		clang -m32 hello_world.c -o 32bits
		nasm -f elf64 hello.s -o hello_asm.o && ld hello_asm.o -o asm

fclean: clean
	rm -f nopie normal 32bits asm

clean:
	rm -f hello_asm.o
