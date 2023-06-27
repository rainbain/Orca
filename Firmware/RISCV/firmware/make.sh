riscv64-unknown-elf-gcc -mabi=ilp32 -march=rv32i -Wl,-Bstatic,-T,sections.lds,--strip-debug -ffreestanding -o bin/firmware.elf -nostdlib start.s SystemAPI.c main.c
objcopy -O ihex bin/firmware.elf bin/firmware.hex
objcopy -I ihex -O binary bin/firmware.hex bin/firmware.bin
hexdump -v -e '/4 "%08x\n"' bin/firmware.bin > bin/firmware.mem
