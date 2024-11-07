nasm -f bin injected_code.asm -o injected_code
xxd -i injected_code
rm -rf injected_code