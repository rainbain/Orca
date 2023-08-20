cmake_minimum_required(VERSION 3.0)

project(OrcaBoot C ASM)

set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_ASM_COMPILER "aarch64-linux-gnu-as")
set(CMAKE_C_FLAGS "-nostdlib -nostartfiles -ffreestanding")

add_executable(boot.elf start.s OrcaBoot.c Fox.c KernelLoader.c SplashScreen.c Init.c)

set_target_properties(boot.elf PROPERTIES LINK_FLAGS "-T ../sections.ld -s")

add_subdirectory(CFox)

target_link_libraries(boot.elf PRIVATE CFox)

# Generate binary file
add_custom_command(TARGET boot.elf POST_BUILD
    COMMAND aarch64-linux-gnu-objcopy -O binary $<TARGET_FILE:boot.elf> boot.bin
)

# Generate boot script
add_custom_command(TARGET boot.elf POST_BUILD
    COMMAND mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "Boot script" -d ../boot.txt boot.scr
)
