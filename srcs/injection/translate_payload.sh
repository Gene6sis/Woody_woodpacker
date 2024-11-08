echo "injection of 'Hello from injected code!\\n'"
nasm -f bin injected_code.asm -o injected_code
xxd -i injected_code
rm -rf injected_code

echo "injection of message and a jump to original entry point"
nasm -f bin injected_code_jump.asm -o injected_code
xxd -i injected_code
rm -rf injected_code

cp injected_code_jump.asm payload.s
rm -f payload*_C
nasm -f bin -o payload payload.s
size=$(wc -c < payload)
echo "Payload 64bits uses $size bytes"
xxd -i -c 8 < payload > payload_c
rm payload
sed -i 's/0x\([0-9a-f][0-9a-f]\)/\\x\1/g' payload_c
tr -d '\n' < payload_c > payload_c2
mv payload_c2 payload_c
tr -d ' ' < payload_c > payload_c2
mv payload_c2 payload_c
tr -d ',' < payload_c > payload_c2
mv payload_c2 payload_c
sed -i 's/\\xe9\\x[0-9a-f][0-9a-f]\\x[0-9a-f][0-9a-f]\\x[0-9a-f][0-9a-f]\\x[0-9a-f][0-9a-f]/\n\n&\n\n/g' payload_c
mv payload_c payload64_C
