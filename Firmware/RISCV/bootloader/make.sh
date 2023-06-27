riscv64-unknown-elf-gcc -mabi=ilp32 -march=rv32i -Wl,-Bstatic,-T,sections.lds,--strip-debug -ffreestanding -o bin/bootloader.elf -nostdlib start.s main.c
objcopy -O ihex bin/bootloader.elf bin/bootloader.hex
objcopy -I ihex -O binary bin/bootloader.hex bin/bootloader.bin
hexdump -v -e '/4 "%08x\n"' bin/bootloader.bin > bin/bootloader.mem
